// $Id$ -*- c++ -*-
// DO NOT MODIFY THIS FILE - it is generated using jikespg on java.g.
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 2001, 2002 International Business
// Machines Corporation and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#ifndef HEADERS

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif

void Parser::InitRuleAction()
{
    rule_action[0] = &Parser::BadAction;
#else // HEADERS
    AstType* MakeArrayType(int tokennum);
    AstSimpleName* MakeSimpleName(int tokennum);
    AstArguments* MakeArguments(int tokennum);
    void MakeLocalVariable(AstModifiers* modifiers, AstType* type,
                           AstListNode* variables);
    AstStatement* MakeSwitchBlockStatement(AstListNode* labels,
                                           AstListNode* statements = NULL);
    void MakeMethodInvocation(int tokennum);
    void MakeCastExpression(AstType* type, int tokennum);

    void BadAction();
    void NoAction();
    void NullAction();
    void SetSym1ToSym2();
    void StartList();
    void AddList2();
    void AddList3();
    void MakeArrayType();
    void MakeClassBody();
    void MakeQualifiedSuper();
    void MakeArrayInitializer();
    void MakeLocalVariable();
    void MakeQualifiedNew();
    void MakeMethodDeclaration();
    void MakeMethodHeader();
    void MakeMethodDeclarator();
    void MakeConstructorDeclaration();
    void MakeLabeledStatement();
    void MakeExpressionStatement();
    void MakeIfThenElseStatement();
    void MakeSwitchLabel();
    void MakeWhileStatement();
    void MakeForStatement();
    void MakeAssertStatement();
    void MakeTryStatement();
    void MakeParenthesizedExpression();
    void MakeClassLiteral();
    void MakeArrayCreationUninitialized();
    void MakeFieldAccess();
    void MakeSuperFieldAccess();
    void MakeQualifiedSuperFieldAccess();
    void MakePreUnaryExpression();
    void MakeCastExpression();
    void MakeArrayAccess();
    void MakeBinaryExpression();
    void MakeInstanceofExpression();
    void MakeConditionalExpression();
#endif // HEADERS

#ifndef HEADERS
    rule_action[1] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[2] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[3] = &Parser::Act3;
#else
    void Act3();
#endif

#ifndef HEADERS
    rule_action[4] = &Parser::Act4;
#else
    void Act4();
#endif

#ifndef HEADERS
    rule_action[5] = &Parser::Act5;
#else
    void Act5();
#endif

#ifndef HEADERS
    rule_action[6] = &Parser::Act6;
#else
    void Act6();
#endif

#ifndef HEADERS
    rule_action[7] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[8] = &Parser::Act8;
#else
    void Act8();
#endif

#ifndef HEADERS
    rule_action[9] = &Parser::Act9;
#else
    void Act9();
#endif

#ifndef HEADERS
    rule_action[10] = &Parser::Act10;
#else
    void Act10();
#endif

#ifndef HEADERS
    rule_action[11] = &Parser::Act11;
#else
    void Act11();
#endif

#ifndef HEADERS
    rule_action[12] = &Parser::Act12;
#else
    void Act12();
#endif

#ifndef HEADERS
    rule_action[13] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[14] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[15] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[16] = &Parser::Act16;
#else
    void Act16();
#endif

#ifndef HEADERS
    rule_action[17] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[18] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[19] = &Parser::Act19;
#else
    void Act19();
#endif

#ifndef HEADERS
    rule_action[20] = &Parser::Act20;
#else
    void Act20();
#endif

#ifndef HEADERS
    rule_action[21] = &Parser::Act21;
#else
    void Act21();
#endif

#ifndef HEADERS
    rule_action[22] = &Parser::Act22;
#else
    void Act22();
#endif

#ifndef HEADERS
    rule_action[23] = &Parser::Act23;
#else
    void Act23();
#endif

#ifndef HEADERS
    rule_action[24] = &Parser::Act24;
#else
    void Act24();
#endif

#ifndef HEADERS
    rule_action[25] = &Parser::Act25;
#else
    void Act25();
#endif

#ifndef HEADERS
    rule_action[26] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[27] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[28] = &Parser::Act28;
#else
    void Act28();
#endif

#ifndef HEADERS
    rule_action[29] = &Parser::MakeArrayType;
#endif

#ifndef HEADERS
    rule_action[30] = &Parser::MakeArrayType;
#endif

#ifndef HEADERS
    rule_action[31] = &Parser::Act31;
#else
    void Act31();
#endif

#ifndef HEADERS
    rule_action[32] = &Parser::MakeFieldAccess;
#endif

#ifndef HEADERS
    rule_action[33] = &Parser::Act33;
#else
    void Act33();
#endif

#ifndef HEADERS
    rule_action[34] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[35] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[36] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[37] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[38] = &Parser::Act38;
#else
    void Act38();
#endif

#ifndef HEADERS
    rule_action[39] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[40] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[41] = &Parser::Act41;
#else
    void Act41();
#endif

#ifndef HEADERS
    rule_action[42] = &Parser::Act42;
#else
    void Act42();
#endif

#ifndef HEADERS
    rule_action[43] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[44] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[45] = &Parser::Act45;
#else
    void Act45();
#endif

#ifndef HEADERS
    rule_action[46] = &Parser::Act46;
#else
    void Act46();
#endif

#ifndef HEADERS
    rule_action[47] = &Parser::Act47;
#else
    void Act47();
#endif

#ifndef HEADERS
    rule_action[48] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[49] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[50] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[51] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[52] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[53] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[54] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[55] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[56] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[57] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[58] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[59] = &Parser::Act59;
#else
    void Act59();
#endif

#ifndef HEADERS
    rule_action[60] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[61] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[62] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[63] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[64] = &Parser::MakeClassBody;
#endif

#ifndef HEADERS
    rule_action[65] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[66] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[67] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[68] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[69] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[70] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[71] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[72] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[73] = &Parser::Act73;
#else
    void Act73();
#endif

#ifndef HEADERS
    rule_action[74] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[75] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[76] = &Parser::Act76;
#else
    void Act76();
#endif

#ifndef HEADERS
    rule_action[77] = &Parser::Act77;
#else
    void Act77();
#endif

#ifndef HEADERS
    rule_action[78] = &Parser::Act78;
#else
    void Act78();
#endif

#ifndef HEADERS
    rule_action[79] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[80] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[81] = &Parser::MakeMethodDeclaration;
#endif

#ifndef HEADERS
    rule_action[82] = &Parser::MakeMethodDeclaration;
#endif

#ifndef HEADERS
    rule_action[83] = &Parser::MakeMethodHeader;
#endif

#ifndef HEADERS
    rule_action[84] = &Parser::Act84;
#else
    void Act84();
#endif

#ifndef HEADERS
    rule_action[85] = &Parser::MakeMethodDeclarator;
#endif

#ifndef HEADERS
    rule_action[86] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[87] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[88] = &Parser::Act88;
#else
    void Act88();
#endif

#ifndef HEADERS
    rule_action[89] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[90] = &Parser::Act90;
#else
    void Act90();
#endif

#ifndef HEADERS
    rule_action[91] = &Parser::Act91;
#else
    void Act91();
#endif

#ifndef HEADERS
    rule_action[92] = &Parser::MakeConstructorDeclaration;
#endif

#ifndef HEADERS
    rule_action[93] = &Parser::MakeMethodDeclarator;
#endif

#ifndef HEADERS
    rule_action[94] = &Parser::Act94;
#else
    void Act94();
#endif

#ifndef HEADERS
    rule_action[95] = &Parser::Act95;
#else
    void Act95();
#endif

#ifndef HEADERS
    rule_action[96] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[97] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[98] = &Parser::Act98;
#else
    void Act98();
#endif

#ifndef HEADERS
    rule_action[99] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[100] = &Parser::MakeClassBody;
#endif

#ifndef HEADERS
    rule_action[101] = &Parser::Act101;
#else
    void Act101();
#endif

#ifndef HEADERS
    rule_action[102] = &Parser::Act102;
#else
    void Act102();
#endif

#ifndef HEADERS
    rule_action[103] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[104] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[105] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[106] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[107] = &Parser::Act107;
#else
    void Act107();
#endif

#ifndef HEADERS
    rule_action[108] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[109] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[110] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[111] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[112] = &Parser::Act112;
#else
    void Act112();
#endif

#ifndef HEADERS
    rule_action[113] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[114] = &Parser::Act114;
#else
    void Act114();
#endif

#ifndef HEADERS
    rule_action[115] = &Parser::MakeLocalVariable;
#endif

#ifndef HEADERS
    rule_action[116] = &Parser::MakeLocalVariable;
#endif

#ifndef HEADERS
    rule_action[117] = &Parser::MakeLocalVariable;
#endif

#ifndef HEADERS
    rule_action[118] = &Parser::Act118;
#else
    void Act118();
#endif

#ifndef HEADERS
    rule_action[119] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[120] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[121] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[122] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[123] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[124] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[125] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[126] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[127] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[128] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[129] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[130] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[131] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[132] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[133] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[134] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[135] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[136] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[137] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[138] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[139] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[140] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[141] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[142] = &Parser::Act142;
#else
    void Act142();
#endif

#ifndef HEADERS
    rule_action[143] = &Parser::MakeLabeledStatement;
#endif

#ifndef HEADERS
    rule_action[144] = &Parser::MakeLabeledStatement;
#endif

#ifndef HEADERS
    rule_action[145] = &Parser::Act145;
#else
    void Act145();
#endif

#ifndef HEADERS
    rule_action[146] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[147] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[148] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[149] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[150] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[151] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[152] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[153] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[154] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[155] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[156] = &Parser::Act156;
#else
    void Act156();
#endif

#ifndef HEADERS
    rule_action[157] = &Parser::Act157;
#else
    void Act157();
#endif

#ifndef HEADERS
    rule_action[158] = &Parser::Act158;
#else
    void Act158();
#endif

#ifndef HEADERS
    rule_action[159] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[160] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[161] = &Parser::Act161;
#else
    void Act161();
#endif

#ifndef HEADERS
    rule_action[162] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[163] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[164] = &Parser::MakeSwitchLabel;
#endif

#ifndef HEADERS
    rule_action[165] = &Parser::MakeSwitchLabel;
#endif

#ifndef HEADERS
    rule_action[166] = &Parser::MakeWhileStatement;
#endif

#ifndef HEADERS
    rule_action[167] = &Parser::MakeWhileStatement;
#endif

#ifndef HEADERS
    rule_action[168] = &Parser::Act168;
#else
    void Act168();
#endif

#ifndef HEADERS
    rule_action[169] = &Parser::MakeForStatement;
#endif

#ifndef HEADERS
    rule_action[170] = &Parser::MakeForStatement;
#endif

#ifndef HEADERS
    rule_action[171] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[172] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[173] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[174] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[175] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[176] = &Parser::MakeAssertStatement;
#endif

#ifndef HEADERS
    rule_action[177] = &Parser::MakeAssertStatement;
#endif

#ifndef HEADERS
    rule_action[178] = &Parser::Act178;
#else
    void Act178();
#endif

#ifndef HEADERS
    rule_action[179] = &Parser::Act179;
#else
    void Act179();
#endif

#ifndef HEADERS
    rule_action[180] = &Parser::Act180;
#else
    void Act180();
#endif

#ifndef HEADERS
    rule_action[181] = &Parser::Act181;
#else
    void Act181();
#endif

#ifndef HEADERS
    rule_action[182] = &Parser::Act182;
#else
    void Act182();
#endif

#ifndef HEADERS
    rule_action[183] = &Parser::MakeTryStatement;
#endif

#ifndef HEADERS
    rule_action[184] = &Parser::MakeTryStatement;
#endif

#ifndef HEADERS
    rule_action[185] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[186] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[187] = &Parser::Act187;
#else
    void Act187();
#endif

#ifndef HEADERS
    rule_action[188] = &Parser::Act188;
#else
    void Act188();
#endif

#ifndef HEADERS
    rule_action[189] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[190] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[191] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[192] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[193] = &Parser::Act193;
#else
    void Act193();
#endif

#ifndef HEADERS
    rule_action[194] = &Parser::MakeParenthesizedExpression;
#endif

#ifndef HEADERS
    rule_action[195] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[196] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[197] = &Parser::Act197;
#else
    void Act197();
#endif

#ifndef HEADERS
    rule_action[198] = &Parser::MakeClassLiteral;
#endif

#ifndef HEADERS
    rule_action[199] = &Parser::MakeClassLiteral;
#endif

#ifndef HEADERS
    rule_action[200] = &Parser::Act200;
#else
    void Act200();
#endif

#ifndef HEADERS
    rule_action[201] = &Parser::Act201;
#else
    void Act201();
#endif

#ifndef HEADERS
    rule_action[202] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[203] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[204] = &Parser::Act204;
#else
    void Act204();
#endif

#ifndef HEADERS
    rule_action[205] = &Parser::MakeQualifiedNew;
#endif

#ifndef HEADERS
    rule_action[206] = &Parser::MakeQualifiedNew;
#endif

#ifndef HEADERS
    rule_action[207] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[208] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[209] = &Parser::MakeArrayCreationUninitialized;
#endif

#ifndef HEADERS
    rule_action[210] = &Parser::MakeArrayCreationUninitialized;
#endif

#ifndef HEADERS
    rule_action[211] = &Parser::Act211;
#else
    void Act211();
#endif

#ifndef HEADERS
    rule_action[212] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[213] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[214] = &Parser::Act214;
#else
    void Act214();
#endif

#ifndef HEADERS
    rule_action[215] = &Parser::Act215;
#else
    void Act215();
#endif

#ifndef HEADERS
    rule_action[216] = &Parser::Act216;
#else
    void Act216();
#endif

#ifndef HEADERS
    rule_action[217] = &Parser::MakeFieldAccess;
#endif

#ifndef HEADERS
    rule_action[218] = &Parser::MakeSuperFieldAccess;
#endif

#ifndef HEADERS
    rule_action[219] = &Parser::MakeQualifiedSuperFieldAccess;
#endif

#ifndef HEADERS
    rule_action[220] = &Parser::Act220;
#else
    void Act220();
#endif

#ifndef HEADERS
    rule_action[221] = &Parser::Act221;
#else
    void Act221();
#endif

#ifndef HEADERS
    rule_action[222] = &Parser::Act222;
#else
    void Act222();
#endif

#ifndef HEADERS
    rule_action[223] = &Parser::Act223;
#else
    void Act223();
#endif

#ifndef HEADERS
    rule_action[224] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[225] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[226] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[227] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[228] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[229] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[230] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[231] = &Parser::Act231;
#else
    void Act231();
#endif

#ifndef HEADERS
    rule_action[232] = &Parser::Act232;
#else
    void Act232();
#endif

#ifndef HEADERS
    rule_action[233] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[234] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[235] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[236] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[237] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[238] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[239] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[240] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[241] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[242] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[243] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[244] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[245] = &Parser::Act245;
#else
    void Act245();
#endif

#ifndef HEADERS
    rule_action[246] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[247] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[248] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[249] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[250] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[251] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[252] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[253] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[254] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[255] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[256] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[257] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[258] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[259] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[260] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[261] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[262] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[263] = &Parser::MakeInstanceofExpression;
#endif

#ifndef HEADERS
    rule_action[264] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[265] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[266] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[267] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[268] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[269] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[270] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[271] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[272] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[273] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[274] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[275] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[276] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[277] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[278] = &Parser::MakeConditionalExpression;
#endif

#ifndef HEADERS
    rule_action[279] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[280] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[281] = &Parser::Act281;
#else
    void Act281();
#endif

#ifndef HEADERS
    rule_action[282] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[283] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[284] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[285] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[286] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[287] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[288] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[289] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[290] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[291] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[292] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[293] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[294] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[295] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[296] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[297] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[298] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[299] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[300] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[301] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[302] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[303] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[304] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[305] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[306] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[307] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[308] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[309] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[310] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[311] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[312] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[313] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[314] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[315] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[316] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[317] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[318] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[319] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[320] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[321] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[322] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[323] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[324] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[325] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[326] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[327] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[328] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[329] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[330] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[331] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[332] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[333] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[334] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[335] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[336] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[337] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[338] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[339] = &Parser::Act339;
#else
    void Act339();
#endif

#ifndef HEADERS
    rule_action[340] = &Parser::Act340;
#else
    void Act340();
#endif

#ifndef HEADERS
    rule_action[341] = &Parser::NullAction;
#endif

#ifndef HEADERS
    return;
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // ! HEADERS
