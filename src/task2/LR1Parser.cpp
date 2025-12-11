/*
 * @file LR1Parser.cpp
 * @id lr1parser-cpp
 * @brief 支持冲突策略与优先移进终结符配置，输出解析步骤与语义树
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task2/LR1Parser.h"
#include "task2/configconstants.h"
#include <QDebug>

/**
 * @brief 获取指定状态和符号的动作
 * @param t LR1动作表
 * @param st 当前状态
 * @param a 当前符号
 * @return 动作字符串，如"s10"、"r5"、"acc"或空字符串（表示无动作）
 * @note 如果没有找到动作，会打印详细的调试信息，包括可用动作和期望的符号
 */
static QString actionFor(const LR1ActionTable& t, int st, const QString& a)
{
    QString action = t.action.value(st).value(a);
    qDebug() << "actionFor - state:" << st << "token:" << a << "action:" << action;
    
    // 如果没有找到动作，打印详细的调试信息
    if (action.isEmpty()) {
        qDebug() << "  No action found for state:" << st << "and token:" << a;
        
        // 检查该状态是否存在
        if (t.action.contains(st)) {
            qDebug() << "  Available actions for state" << st << ":";
            const auto& symbolActions = t.action[st];
            QStringList availableTokens;
            for (auto symbolIt = symbolActions.constBegin(); symbolIt != symbolActions.constEnd(); ++symbolIt) {
                availableTokens.append(symbolIt.key());
                qDebug() << "    " << symbolIt.key() << " -> " << symbolIt.value();
            }
            qDebug() << "  Expected tokens: " << availableTokens.join(", ");
        } else {
            qDebug() << "  State" << st << "not found in action table";
        }
    }
    
    return action;
}

/**
 * @brief 获取指定状态和非终结符的goto转移
 * @param t LR1动作表
 * @param st 当前状态
 * @param A 当前非终结符
 * @return goto转移的目标状态，-1表示无转移
 */
static int gotoFor(const LR1ActionTable& t, int st, const QString& A)
{
    return t.gotoTable.value(st).value(A, -1);
}

/**
 * @brief 解析归约动作，获取产生式的左部和右部
 * @param t LR1动作表
 * @param act 动作字符串，如"r5"或"r A -> B C"
 * @param L 输出参数，产生式的左部
 * @param rhs 输出参数，产生式的右部符号列表
 * @return 是否成功解析
 */
static bool parseReduction(const LR1ActionTable& t,
                           const QString&        act,
                           QString&              L,
                           QVector<QString>&     rhs)
{
    QString s   = act.mid(1).trimmed();
    bool    ok  = false;
    int     idx = s.toInt(&ok);
    if (ok)
    {
        for (const auto& pr : t.reductions)
        {
            if (pr.first == idx)
            {
                QString text  = pr.second;
                int     arrow = text.indexOf("->");
                L             = text.left(arrow).trimmed();
                QString R     = text.mid(arrow + 2).trimmed();
                rhs.clear();
                if (!R.isEmpty())
                {
                    for (auto x : R.split(' ', Qt::SkipEmptyParts)) rhs.push_back(x.trimmed());
                }
                return true;
            }
        }
    }
    int arrow = s.indexOf("->");
    if (arrow >= 0)
    {
        L         = s.left(arrow).trimmed();
        QString R = s.mid(arrow + 2).trimmed();
        rhs.clear();
        if (!R.isEmpty())
        {
            for (auto x : R.split(' ', Qt::SkipEmptyParts)) rhs.push_back(x.trimmed());
        }
        return true;
    }
    return false;
}

/**
 * @brief 根据产生式的左部和右部获取归约编号
 * @param t LR1动作表
 * @param L 产生式的左部
 * @param rhs 产生式的右部符号列表
 * @return 归约编号，-1表示未找到
 */
static int reductionIdFor(const LR1ActionTable& t, const QString& L, const QVector<QString>& rhs)
{
    QString key = L + " -> " + (rhs.isEmpty() ? QString("#") : rhs.join(" "));
    for (const auto& pr : t.reductions)
    {
        if (pr.second == key)
            return pr.first;
    }
    return -1;
}

/**
 * @brief LR1Parser构造函数
 * @param parent 父对象
 */
LR1Parser::LR1Parser(QObject *parent) : QObject(parent)
{
}

/**
 * @brief 将解析步骤添加到结果中，并发出信号
 * @param steps 解析步骤列表
 * @param stepIdx 步骤索引
 * @param stk 当前栈状态
 * @param rest 剩余输入符号
 * @param act 当前动作
 * @param prod 当前使用的产生式
 * @param parser LR1Parser实例，用于发出信号
 */
static void pushStep(QVector<ParseStep>&                 steps,
                     int                                 stepIdx,
                     const QVector<QPair<int, QString>>& stk,
                     const QVector<TokenInfo>&           rest,
                     const QString&                      act,
                     const QString&                      prod,
                     LR1Parser*                          parser)
{
    ParseStep ps;
    ps.step       = stepIdx;
    ps.stack      = stk;
    ps.rest       = rest;
    ps.action     = act;
    ps.production = prod;
    steps.push_back(ps);
    
    // 发出信号，通知UI更新
    if (parser)
    {
        parser->stepUpdated(ps);
    }
}

/**
 * @brief 执行LR1语法分析
 * @param tokens 词法分析结果，token列表
 * @param g 文法，未使用
 * @param t LR1动作表
 * @return 解析结果，包含解析步骤、语法树和错误信息
 * @note 该方法只进行语法分析，不生成语义树
 */
ParseResult LR1Parser::parse(const QVector<TokenInfo>& tokens,
                             const Grammar&          g,
                             const LR1ActionTable&   t)
{
    Q_UNUSED(g); // 标记参数 g 为未使用，避免编译警告
    ParseResult      res;
    QVector<TokenInfo> input = tokens;
    
    // 添加文件结束符
    TokenInfo eofToken;
    eofToken.tokenType = "$";
    eofToken.lexeme = "";
    input.push_back(eofToken);
    
    QVector<QPair<int, QString>> stack;
    QVector<ParseTreeNode*>      nodeStk;
    stack.push_back({0, QString()});
    int step = 0;
    while (!input.isEmpty())
    {
        QString a   = input[0].tokenType; // 只使用token类型进行语法分析，词素不参与
        int     st  = stack.isEmpty() ? -1 : stack.back().first;
        QString act = actionFor(t, st, a);
        if (act.contains('|'))
        {
            auto parts  = act.split('|');
            bool hasAcc = false;
            for (auto p : parts)
                if (p == QStringLiteral("acc"))
                {
                    hasAcc = true;
                    break;
                }
            if (hasAcc)
            {
                act = QStringLiteral("acc");
            }
            else
            {
                QString policy  = ConfigConstants::lr1ConflictPolicy().trimmed().toLower();
                auto    prefer  = ConfigConstants::lr1PreferShiftTokens();
                QString nextTok = a;
                if (!prefer.isEmpty())
                {
                    for (const auto& pt : prefer)
                    {
                        if (nextTok == pt)
                        {
                            QString pick;
                            for (auto p : parts)
                                if (p.startsWith("s"))
                                {
                                    pick = p;
                                    break;
                                }
                            if (!pick.isEmpty())
                            {
                                act = pick;
                                break;
                            }
                        }
                    }
                }
                if (act.isEmpty())
                {
                    if (policy == "prefer_shift")
                    {
                        QString pick;
                        for (auto p : parts)
                            if (p.startsWith("s"))
                            {
                                pick = p;
                                break;
                            }
                        if (pick.isEmpty())
                            pick = parts[0];
                        act = pick;
                    }
                    else if (policy == "prefer_reduce")
                    {
                        QString pick;
                        for (auto p : parts)
                            if (p.startsWith("r"))
                            {
                                pick = p;
                                break;
                            }
                        if (pick.isEmpty())
                            pick = parts[0];
                        act = pick;
                    }
                    else
                    {
                        QString msg = QString("错误：状态=%1, 前瞻=%2, 动作冲突未配置，中止").arg(st).arg(a);
                        pushStep(res.steps, step++, stack, input, QStringLiteral("error"), msg, this);
                        res.errorPos = res.steps.size();
                        res.errorMsg = msg;
                        break;
                    }
                }
            }
        }
        if (act.isEmpty())
        {
            // 错误触发点后移：若上一步为移进，则用当前lookahead的下一符号重试一次
            if (!res.steps.isEmpty() && res.steps.back().action.startsWith("s") && input.size() > 1)
            {
                QString a2   = input[1].tokenType; // 只使用token类型进行错误恢复
                QString act2 = actionFor(t, st, a2);
                if (!act2.isEmpty())
                    act = act2;
            }
            if (act.isEmpty())
            {
                // 收集可用的期望token，便于调试
                QStringList expectedTokens;
                if (t.action.contains(st)) {
                    const auto& symbolActions = t.action[st];
                    for (auto symbolIt = symbolActions.constBegin(); symbolIt != symbolActions.constEnd(); ++symbolIt) {
                        expectedTokens.append(symbolIt.key());
                    }
                }
                
                QString msg = QString("错误：状态=%1, 前瞻=%2, 无可用动作，中止").arg(st).arg(a);
                if (!expectedTokens.isEmpty()) {
                    msg += QString("，期望的token: %1").arg(expectedTokens.join(", "));
                }
                pushStep(res.steps, step++, stack, input, QStringLiteral("error"), msg, this);
                res.errorPos = res.steps.size();
                res.errorMsg = msg;
                break;
            }
        }
        if (act == "acc")
        {
            pushStep(res.steps, step++, stack, input, act, QString(), this);
            if (!nodeStk.isEmpty())
                res.root = nodeStk.back();
            break;
        }
        if (act.startsWith("s"))
        {
            int to = act.mid(1).toInt();
            stack.push_back({to, a});
            ParseTreeNode* n = new ParseTreeNode;
            n->symbol        = a;
            res.root         = n;
            nodeStk.push_back(n);
            // 语义过程记录：移进叶子
            pushStep(res.semanticSteps, step, stack, input, QString("shift %1").arg(a), QString(), this);
            pushStep(res.steps, step++, stack, input, act, QString(), this);
            input.pop_front();
            continue;
        }
        if (act.startsWith("r"))
        {
            QString          L;
            QVector<QString> rhs;
            if (!parseReduction(t, act, L, rhs))
            {
                QString msg = QString("错误：归约解析失败，动作=%1，中止").arg(act);
                pushStep(res.steps, step++, stack, input, QStringLiteral("error"), msg, this);
                res.errorPos = res.steps.size();
                break;
            }
            int                     k = rhs.isEmpty() ? 0 : rhs.size();
            QVector<ParseTreeNode*> kids;
            for (int i = 0; i < k; ++i)
            {
                if (!stack.isEmpty())
                    stack.pop_back();
                if (!nodeStk.isEmpty())
                {
                    kids.push_back(nodeStk.back());
                    nodeStk.pop_back();
                }
            }
            std::reverse(kids.begin(), kids.end());
            int stTop = stack.isEmpty() ? -1 : stack.back().first;
            int to    = gotoFor(t, stTop, L);
            if (to < 0)
            {
                QString msg = QString("错误：goto 失败，状态=%1, 归约到=%2，中止").arg(stTop).arg(L);
                pushStep(res.steps, step++, stack, input, QStringLiteral("error"), msg, this);
                res.errorPos = res.steps.size();
                break;
            }
            stack.push_back({to, L});
            ParseTreeNode* p = new ParseTreeNode;
            p->symbol        = L;
            p->children      = kids;
            nodeStk.push_back(p);
            res.root = p;
            QString kidsStr;
            for (int i = 0; i < kids.size(); ++i)
                kidsStr += (i ? "," : "") + (kids[i] ? kids[i]->symbol : QString());
            pushStep(res.semanticSteps,
                     step,
                     stack,
                     input,
                     QString("reduce %1 -> %2, children=[%3]")
                         .arg(L)
                         .arg(rhs.isEmpty() ? QString("#") : rhs.join(" "))
                         .arg(kidsStr),
                     QString(),
                     this);
            pushStep(res.steps,
                     step++,
                     stack,
                     input,
                     act,
                     QString("%1 -> %2").arg(L).arg(rhs.isEmpty() ? QString("#") : rhs.join(" ")),
                     this);
            continue;
        }
        pushStep(res.steps,
                 step++,
                 stack,
                 input,
                 QStringLiteral("error"),
                 QString("错误：未知动作 '%1'，中止").arg(act),
                 this);
        res.errorPos = res.steps.size();
        break;
    }
    return res;
}

/**
 * @brief 创建一个新的语义AST节点
 * @param tag 节点标签
 * @return 指向新创建节点的指针
 */
static SemanticASTNode* makeSemNode(const QString& tag)
{
    auto n = new SemanticASTNode();
    n->tag = tag;
    return n;
}

/**
 * @brief 构建语义AST
 * @param L 产生式左部
 * @param semKids 语义子节点列表
 * @param roles 子节点角色列表
 * @param roleMeaning 角色含义映射
 * @param rootPolicy 根节点选择策略
 * @param childOrder 子节点顺序
 * @return 构建的语义AST根节点
 */
static SemanticASTNode* buildSemantic(const QString&                   L,
                                      const QVector<SemanticASTNode*>& semKids,
                                      const QVector<int>&              roles,
                                      const QMap<int, QString>&        roleMeaning,
                                      const QString&                   rootPolicy,
                                      const QString&                   childOrder)
{
    SemanticASTNode* root = nullptr;
    SemanticASTNode* currentRoot = nullptr;
    
    // 遍历所有子树和对应的角色
    for (int i = 0; i < semKids.size(); ++i)
    {
        SemanticASTNode* child = semKids[i];
        if (!child) continue;
        
        int role = (i < roles.size()) ? roles[i] : 0;
        QString roleName = roleMeaning.value(role, "skip");
        
        if (roleName == "skip")
        {
            // 规则1: 0=跳过该子树，不添加到结果中
            continue;
        }
        else if (roleName == "root")
        {
            // 规则2: 1=提升为根节点
            if (!root)
            {
                // 第一个角色1的节点作为根节点
                root = child;
                currentRoot = child;
            }
            else
            {
                // 后续角色1的节点作为兄弟节点添加到根节点
                root->children.push_back(child);
                // 更新当前根节点为新的角色1节点，处理后续的角色2节点
                currentRoot = child;
            }
        }
        else if (roleName == "child")
        {
            // 规则3: 2=作为最近的角色1节点的子节点
            if (currentRoot)
            {
                // 添加到最近的角色1节点
                currentRoot->children.push_back(child);
            }
            else if (root)
            {
                // 如果没有角色1节点，添加到根节点
                root->children.push_back(child);
            }
        }
    }
    
    // 如果没有找到任何节点，创建一个默认根节点
    if (!root)
    {
        root = makeSemNode(L);
    }
    
    return root;
}

/**
 * @brief 执行带语义动作的LR1语法分析
 * @param tokens 词法分析结果，token列表
 * @param g 文法
 * @param t LR1动作表
 * @param actions 语义动作配置
 * @param roleMeaning 角色含义映射
 * @param rootPolicy 根节点选择策略
 * @param childOrder 子节点顺序
 * @return 解析结果，包含解析步骤、语法树、语义树和错误信息
 * @note 该方法不仅进行语法分析，还生成语义树
 */
ParseResult LR1Parser::parseWithSemantics(const QVector<TokenInfo>&                    tokens,
                                          const Grammar&                              g,
                                          const LR1ActionTable&                       t,
                                          const QMap<QString, QVector<QVector<int>>>& actions,
                                          const QMap<int, QString>&                   roleMeaning,
                                          const QString&                              rootPolicy,
                                          const QString&                              childOrder)
{
    ParseResult      res;
    QVector<TokenInfo> input = tokens;
    
    // 添加文件结束符
    TokenInfo eofToken;
    eofToken.tokenType = "$";
    eofToken.lexeme = "";
    input.push_back(eofToken);
    
    QVector<QPair<int, QString>> stack;
    QVector<ParseTreeNode*>      nodeStk;
    QVector<SemanticASTNode*>    semStk;
    stack.push_back({0, QString()});
    int step = 0;
    
    QSet<QString> idNames;
    for (auto s : ConfigConstants::identifierTokenNames()) idNames.insert(s.trimmed().toLower());
    
    while (!input.isEmpty())
    {
        QString a   = input[0].tokenType; // 只使用token类型进行语法分析
        int     st  = stack.isEmpty() ? -1 : stack.back().first;
        QString act = actionFor(t, st, a);
        if (act.contains('|'))
        {
            auto parts  = act.split('|');
            bool hasAcc = false;
            for (auto p : parts)
                if (p == QStringLiteral("acc"))
                {
                    hasAcc = true;
                    break;
                }
            if (hasAcc)
            {
                act = QStringLiteral("acc");
            }
            else
            {
                QString policy  = ConfigConstants::lr1ConflictPolicy().trimmed().toLower();
                auto    prefer  = ConfigConstants::lr1PreferShiftTokens();
                QString nextTok = a;
                if (!prefer.isEmpty())
                {
                    for (const auto& pt : prefer)
                    {
                        if (nextTok == pt)
                        {
                            QString pick;
                            for (auto p : parts)
                                if (p.startsWith("s"))
                                {
                                    pick = p;
                                    break;
                                }
                            if (!pick.isEmpty())
                            {
                                act = pick;
                                break;
                            }
                        }
                    }
                }
                if (act.isEmpty())
                {
                    if (policy == "prefer_shift")
                    {
                        QString pick;
                        for (auto p : parts)
                            if (p.startsWith("s"))
                            {
                                pick = p;
                                break;
                            }
                        if (pick.isEmpty())
                            pick = parts[0];
                        act = pick;
                    }
                    else if (policy == "prefer_reduce")
                    {
                        QString pick;
                        for (auto p : parts)
                            if (p.startsWith("r"))
                            {
                                pick = p;
                                break;
                            }
                        if (pick.isEmpty())
                            pick = parts[0];
                        act = pick;
                    }
                    else
                    {
                        QString msg =
                            QString("错误：状态=%1, 前瞻=%2, 动作冲突未配置，中止").arg(st).arg(a);
                        pushStep(res.steps, step++, stack, input, QStringLiteral("error"), msg, this);
                        pushStep(
                            res.semanticSteps, step, stack, input, QStringLiteral("error"), msg, this);
                        res.errorPos = res.steps.size();
                        break;
                    }
                }
            }
        }
        if (act.isEmpty())
        {
            // 错误触发点后移：若上一步为移进，则用当前lookahead的下一符号重试一次
            if (!res.steps.isEmpty() && res.steps.back().action.startsWith("s") && input.size() > 1)
            {
                QString a2   = input[1].tokenType; // 只使用token类型进行错误恢复
                QString act2 = actionFor(t, st, a2);
                if (!act2.isEmpty())
                    act = act2;
            }
            if (act.isEmpty())
            {
                // 收集可用的期望token，便于调试
                QStringList expectedTokens;
                if (t.action.contains(st)) {
                    const auto& symbolActions = t.action[st];
                    for (auto symbolIt = symbolActions.constBegin(); symbolIt != symbolActions.constEnd(); ++symbolIt) {
                        expectedTokens.append(symbolIt.key());
                    }
                }
                
                QString msg = QString("错误：状态=%1, 前瞻=%2, 无可用动作，中止").arg(st).arg(a);
                if (!expectedTokens.isEmpty()) {
                    msg += QString("，期望的token: %1").arg(expectedTokens.join(", "));
                }
                pushStep(res.steps, step++, stack, input, QStringLiteral("error"), msg, this);
                pushStep(res.semanticSteps, step, stack, input, QStringLiteral("error"), msg, this);
                res.errorPos = res.steps.size();
                break;
            }
        }
        if (act == "acc")
        {
            pushStep(res.steps, step++, stack, input, act, QString(), this);
            if (!nodeStk.isEmpty())
                res.root = nodeStk.back();
            if (!semStk.isEmpty())
                res.astRoot = semStk.back();
            // 顶层包装为起始非终结符名称
            if (res.astRoot)
            {
                SemanticASTNode* top = new SemanticASTNode();
                top->tag             = g.startSymbol;
                top->children.push_back(res.astRoot);
                res.astRoot = top;
            }
            break;
        }
        if (act.startsWith("s"))
        {
            int to = act.mid(1).toInt();
            stack.push_back({to, a});
            ParseTreeNode* n = new ParseTreeNode;
            n->symbol        = a;
            res.root         = n;
            nodeStk.push_back(n);
            
            // 语义：对携带词素的终结符使用“token(lexeme)”作为叶子标签
            QString tag  = a;
            QString lexeme = input[0].lexeme; // 直接从TokenInfo获取词素
            QString mlow = a.trimmed().toLower();
            
            if (!lexeme.isEmpty() || idNames.contains(mlow))
            {
                if (!lexeme.isEmpty())
                {
                    tag = QString("%1(%2)").arg(a).arg(lexeme);
                }
            }
            
            semStk.push_back(makeSemNode(tag));
            
            // 语义过程记录：移进叶子（可能替换为词素）
            pushStep(res.semanticSteps,
                     step,
                     stack,
                     input,
                     QString("移进符号[%1]%2，语义栈压入终结符节点")
                         .arg(a)
                         .arg(tag != a ? QString("（lexeme=%1）").arg(lexeme) : QString()),
                     QString(),
                     this);
            
            pushStep(res.steps, step++, stack, input, act, QString(), this);
            input.pop_front();
            continue;
        }
        if (act.startsWith("r"))
        {
            QString          L;
            QVector<QString> rhs;
            if (!parseReduction(t, act, L, rhs))
            {
                QString msg = QString("错误：归约解析失败，动作=%1，中止").arg(act);
                pushStep(res.steps, step++, stack, input, QStringLiteral("error"), msg, this);
                pushStep(res.semanticSteps, step, stack, input, QStringLiteral("error"), msg, this);
                res.errorPos = res.steps.size();
                break;
            }
            int                       k = rhs.isEmpty() ? 0 : rhs.size();
            QVector<ParseTreeNode*>   kids;
            QVector<SemanticASTNode*> semKids;
            for (int i = 0; i < k; ++i)
            {
                if (!stack.isEmpty())
                    stack.pop_back();
                if (!nodeStk.isEmpty())
                {
                    kids.push_back(nodeStk.back());
                    nodeStk.pop_back();
                }
                if (!semStk.isEmpty())
                {
                    semKids.push_back(semStk.back());
                    semStk.pop_back();
                }
            }
            std::reverse(kids.begin(), kids.end());
            std::reverse(semKids.begin(), semKids.end());
            int stTop = stack.isEmpty() ? -1 : stack.back().first;
            int to    = gotoFor(t, stTop, L);
            if (to < 0)
            {
                QString msg =
                    QString("错误：goto 失败，状态=%1, 归约到=%2，中止").arg(stTop).arg(L);
                pushStep(res.steps, step++, stack, input, QStringLiteral("error"), msg, this);
                pushStep(res.semanticSteps, step, stack, input, QStringLiteral("error"), msg, this);
                res.errorPos = res.steps.size();
                break;
            }
            stack.push_back({to, L});
            ParseTreeNode* p = new ParseTreeNode;
            p->symbol        = L;
            p->children      = kids;
            nodeStk.push_back(p);
            res.root = p;
            
            // 语义：按配置与角色位组合
            QVector<int> roles;
            if (actions.contains(L))
            {
                const auto& vec = actions.value(L);
                // 根据实际规约候选（RHS 完整匹配）选择角色位
                int pick = -1;
                if (g.productions.contains(L))
                {
                    const auto& alts = g.productions.value(L);
                    for (int i = 0; i < alts.size(); ++i)
                    {
                        const auto& alt = alts[i];
                        if (alt.right.size() == rhs.size())
                        {
                            bool eq = true;
                            for (int j = 0; j < rhs.size(); ++j)
                            {
                                if (alt.right[j] != rhs[j])
                                {
                                    eq = false;
                                    break;
                                }
                            }
                            if (eq)
                            {
                                pick = i;
                                break;
                            }
                        }
                    }
                }
                if (pick >= 0 && pick < vec.size())
                    roles = vec[pick];
            }
            
            auto sem = buildSemantic(L, semKids, roles, roleMeaning, rootPolicy, childOrder);
            semStk.push_back(sem);
            res.astRoot = sem;
            
            // 语义过程记录：构建语义节点，根与孩子
            QString kidsStr;
            for (int i = 0; i < semKids.size(); ++i)
                kidsStr += (i ? "," : "") + (semKids[i] ? semKids[i]->tag : QString());
            
            int rid = reductionIdFor(t, L, rhs);
            pushStep(res.semanticSteps,
                     step,
                     stack,
                     input,
                     QString("准备归约：产生式 %1 → %2，执行语义动作构建非终结符节点 [%3]%4，子节点=[%5]")
                         .arg(rid >= 0 ? QString::number(rid) : QStringLiteral("?"))
                         .arg(rhs.isEmpty() ? QString("#") : rhs.join(" "))
                         .arg(sem ? sem->tag : L)
                         .arg(rid >= 0 ? QString("（编码:%1）").arg(rid) : QString())
                         .arg(kidsStr),
                     QString(),
                     this);
            
            pushStep(res.steps,
                     step++,
                     stack,
                     input,
                     act,
                     QString("%1 -> %2").arg(L).arg(rhs.isEmpty() ? QString("#") : rhs.join(" ")),
                     this);
            continue;
        }
        pushStep(res.steps,
                 step++,
                 stack,
                 input,
                 QStringLiteral("error"),
                 QString("错误：未知动作 '%1'，中止").arg(act),
                 this);
        pushStep(res.semanticSteps,
                 step,
                 stack,
                 input,
                 QStringLiteral("error"),
                 QString("错误：未知动作 '%1'，中止").arg(act),
                 this);
        res.errorPos = res.steps.size();
        break;
    }
    
    // 确保语义树的根节点始终是起始符号
    if (res.astRoot && res.astRoot->tag != g.startSymbol)
    {
        SemanticASTNode* top = new SemanticASTNode();
        top->tag = g.startSymbol;
        top->children.push_back(res.astRoot);
        res.astRoot = top;
    }
    
    return res;
}