// $Id$ -*- c++ -*-
// DO NOT MODIFY THIS FILE - it is generated using jikespg on java.g.
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 2004 IBM Corporation and others.  All Rights Reserved.
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
    AstName* MakeSimpleName(int tokennum);
    void MakeFormalParameter(AstModifiers* modifiers, AstType* type,
                             int ellipsis_token,
                             AstVariableDeclaratorId* variable);
    AstArguments* MakeArguments(int tokennum);
    void MakeLocalVariable(AstModifiers* modifiers, AstType* type,
                           AstListNode* variables);
    AstBlock* MakeBlock(int tokennum);
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
    void MakeImportDeclaration();
    void MakeClassBody();
    void MakeQualifiedSuper();
    void MakeArrayInitializer();
    void MakeLocalVariable();
    void MakeQualifiedNew();
    void MakeMethodDeclaration();
    void MakeMethodHeader();
    void MakeMethodDeclarator();
    void MakeFormalParameterA();
    void MakeFormalParameterB();
    void MakeConstructorDeclaration();
    void MakeLabeledStatement();
    void MakeExpressionStatement();
    void MakeIfThenElseStatement();
    void MakeSwitchLabel();
    void MakeWhileStatement();
    void MakeForStatement();
    void MakeForeachStatement();
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
    rule_action[32] = &Parser::Act32;
#else
    void Act32();
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
    rule_action[41] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[42] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[43] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[44] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[45] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[46] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[47] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[48] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[49] = &Parser::Act49;
#else
    void Act49();
#endif

#ifndef HEADERS
    rule_action[50] = &Parser::Act50;
#else
    void Act50();
#endif

#ifndef HEADERS
    rule_action[51] = &Parser::Act51;
#else
    void Act51();
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
    rule_action[59] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[60] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[61] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[62] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[63] = &Parser::Act63;
#else
    void Act63();
#endif

#ifndef HEADERS
    rule_action[64] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[65] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[66] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[67] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[68] = &Parser::MakeClassBody;
#endif

#ifndef HEADERS
    rule_action[69] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[70] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[71] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[72] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[73] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[74] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[75] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[76] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[77] = &Parser::Act77;
#else
    void Act77();
#endif

#ifndef HEADERS
    rule_action[78] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[79] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[80] = &Parser::Act80;
#else
    void Act80();
#endif

#ifndef HEADERS
    rule_action[81] = &Parser::Act81;
#else
    void Act81();
#endif

#ifndef HEADERS
    rule_action[82] = &Parser::Act82;
#else
    void Act82();
#endif

#ifndef HEADERS
    rule_action[83] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[84] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[85] = &Parser::MakeMethodDeclaration;
#endif

#ifndef HEADERS
    rule_action[86] = &Parser::MakeMethodDeclaration;
#endif

#ifndef HEADERS
    rule_action[87] = &Parser::MakeMethodHeader;
#endif

#ifndef HEADERS
    rule_action[88] = &Parser::Act88;
#else
    void Act88();
#endif

#ifndef HEADERS
    rule_action[89] = &Parser::MakeMethodDeclarator;
#endif

#ifndef HEADERS
    rule_action[90] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[91] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[92] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[93] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[94] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[95] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[96] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[97] = &Parser::MakeFormalParameterB;
#endif

#ifndef HEADERS
    rule_action[98] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[99] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[100] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[101] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[102] = &Parser::MakeFormalParameterB;
#endif

#ifndef HEADERS
    rule_action[103] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[104] = &Parser::Act104;
#else
    void Act104();
#endif

#ifndef HEADERS
    rule_action[105] = &Parser::Act105;
#else
    void Act105();
#endif

#ifndef HEADERS
    rule_action[106] = &Parser::MakeConstructorDeclaration;
#endif

#ifndef HEADERS
    rule_action[107] = &Parser::MakeMethodDeclarator;
#endif

#ifndef HEADERS
    rule_action[108] = &Parser::Act108;
#else
    void Act108();
#endif

#ifndef HEADERS
    rule_action[109] = &Parser::Act109;
#else
    void Act109();
#endif

#ifndef HEADERS
    rule_action[110] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[111] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[112] = &Parser::Act112;
#else
    void Act112();
#endif

#ifndef HEADERS
    rule_action[113] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[114] = &Parser::MakeClassBody;
#endif

#ifndef HEADERS
    rule_action[115] = &Parser::Act115;
#else
    void Act115();
#endif

#ifndef HEADERS
    rule_action[116] = &Parser::Act116;
#else
    void Act116();
#endif

#ifndef HEADERS
    rule_action[117] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[118] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[119] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[120] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[121] = &Parser::Act121;
#else
    void Act121();
#endif

#ifndef HEADERS
    rule_action[122] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[123] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[124] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[125] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[126] = &Parser::Act126;
#else
    void Act126();
#endif

#ifndef HEADERS
    rule_action[127] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[128] = &Parser::Act128;
#else
    void Act128();
#endif

#ifndef HEADERS
    rule_action[129] = &Parser::MakeLocalVariable;
#endif

#ifndef HEADERS
    rule_action[130] = &Parser::MakeLocalVariable;
#endif

#ifndef HEADERS
    rule_action[131] = &Parser::MakeLocalVariable;
#endif

#ifndef HEADERS
    rule_action[132] = &Parser::Act132;
#else
    void Act132();
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
    rule_action[142] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[143] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[144] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[145] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[146] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[147] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[148] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[149] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[150] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[151] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[152] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[153] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[154] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[155] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[156] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[157] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[158] = &Parser::Act158;
#else
    void Act158();
#endif

#ifndef HEADERS
    rule_action[159] = &Parser::MakeLabeledStatement;
#endif

#ifndef HEADERS
    rule_action[160] = &Parser::MakeLabeledStatement;
#endif

#ifndef HEADERS
    rule_action[161] = &Parser::Act161;
#else
    void Act161();
#endif

#ifndef HEADERS
    rule_action[162] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[163] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[164] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[165] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[166] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[167] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[168] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[169] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[170] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[171] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[172] = &Parser::Act172;
#else
    void Act172();
#endif

#ifndef HEADERS
    rule_action[173] = &Parser::Act173;
#else
    void Act173();
#endif

#ifndef HEADERS
    rule_action[174] = &Parser::Act174;
#else
    void Act174();
#endif

#ifndef HEADERS
    rule_action[175] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[176] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[177] = &Parser::Act177;
#else
    void Act177();
#endif

#ifndef HEADERS
    rule_action[178] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[179] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[180] = &Parser::MakeSwitchLabel;
#endif

#ifndef HEADERS
    rule_action[181] = &Parser::MakeSwitchLabel;
#endif

#ifndef HEADERS
    rule_action[182] = &Parser::MakeWhileStatement;
#endif

#ifndef HEADERS
    rule_action[183] = &Parser::MakeWhileStatement;
#endif

#ifndef HEADERS
    rule_action[184] = &Parser::Act184;
#else
    void Act184();
#endif

#ifndef HEADERS
    rule_action[185] = &Parser::MakeForStatement;
#endif

#ifndef HEADERS
    rule_action[186] = &Parser::MakeForStatement;
#endif

#ifndef HEADERS
    rule_action[187] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[188] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[189] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[190] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[191] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[192] = &Parser::MakeForeachStatement;
#endif

#ifndef HEADERS
    rule_action[193] = &Parser::MakeForeachStatement;
#endif

#ifndef HEADERS
    rule_action[194] = &Parser::MakeAssertStatement;
#endif

#ifndef HEADERS
    rule_action[195] = &Parser::MakeAssertStatement;
#endif

#ifndef HEADERS
    rule_action[196] = &Parser::Act196;
#else
    void Act196();
#endif

#ifndef HEADERS
    rule_action[197] = &Parser::Act197;
#else
    void Act197();
#endif

#ifndef HEADERS
    rule_action[198] = &Parser::Act198;
#else
    void Act198();
#endif

#ifndef HEADERS
    rule_action[199] = &Parser::Act199;
#else
    void Act199();
#endif

#ifndef HEADERS
    rule_action[200] = &Parser::Act200;
#else
    void Act200();
#endif

#ifndef HEADERS
    rule_action[201] = &Parser::MakeTryStatement;
#endif

#ifndef HEADERS
    rule_action[202] = &Parser::MakeTryStatement;
#endif

#ifndef HEADERS
    rule_action[203] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[204] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[205] = &Parser::Act205;
#else
    void Act205();
#endif

#ifndef HEADERS
    rule_action[206] = &Parser::Act206;
#else
    void Act206();
#endif

#ifndef HEADERS
    rule_action[207] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[208] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[209] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[210] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[211] = &Parser::Act211;
#else
    void Act211();
#endif

#ifndef HEADERS
    rule_action[212] = &Parser::MakeParenthesizedExpression;
#endif

#ifndef HEADERS
    rule_action[213] = &Parser::MakeParenthesizedExpression;
#endif

#ifndef HEADERS
    rule_action[214] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[215] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[216] = &Parser::Act216;
#else
    void Act216();
#endif

#ifndef HEADERS
    rule_action[217] = &Parser::MakeClassLiteral;
#endif

#ifndef HEADERS
    rule_action[218] = &Parser::MakeClassLiteral;
#endif

#ifndef HEADERS
    rule_action[219] = &Parser::Act219;
#else
    void Act219();
#endif

#ifndef HEADERS
    rule_action[220] = &Parser::Act220;
#else
    void Act220();
#endif

#ifndef HEADERS
    rule_action[221] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[222] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[223] = &Parser::Act223;
#else
    void Act223();
#endif

#ifndef HEADERS
    rule_action[224] = &Parser::MakeQualifiedNew;
#endif

#ifndef HEADERS
    rule_action[225] = &Parser::MakeQualifiedNew;
#endif

#ifndef HEADERS
    rule_action[226] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[227] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[228] = &Parser::MakeArrayCreationUninitialized;
#endif

#ifndef HEADERS
    rule_action[229] = &Parser::MakeArrayCreationUninitialized;
#endif

#ifndef HEADERS
    rule_action[230] = &Parser::Act230;
#else
    void Act230();
#endif

#ifndef HEADERS
    rule_action[231] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[232] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[233] = &Parser::Act233;
#else
    void Act233();
#endif

#ifndef HEADERS
    rule_action[234] = &Parser::Act234;
#else
    void Act234();
#endif

#ifndef HEADERS
    rule_action[235] = &Parser::Act235;
#else
    void Act235();
#endif

#ifndef HEADERS
    rule_action[236] = &Parser::MakeFieldAccess;
#endif

#ifndef HEADERS
    rule_action[237] = &Parser::MakeSuperFieldAccess;
#endif

#ifndef HEADERS
    rule_action[238] = &Parser::MakeQualifiedSuperFieldAccess;
#endif

#ifndef HEADERS
    rule_action[239] = &Parser::Act239;
#else
    void Act239();
#endif

#ifndef HEADERS
    rule_action[240] = &Parser::Act240;
#else
    void Act240();
#endif

#ifndef HEADERS
    rule_action[241] = &Parser::Act241;
#else
    void Act241();
#endif

#ifndef HEADERS
    rule_action[242] = &Parser::Act242;
#else
    void Act242();
#endif

#ifndef HEADERS
    rule_action[243] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[244] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[245] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[246] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[247] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[248] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[249] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[250] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[251] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[252] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[253] = &Parser::Act253;
#else
    void Act253();
#endif

#ifndef HEADERS
    rule_action[254] = &Parser::Act254;
#else
    void Act254();
#endif

#ifndef HEADERS
    rule_action[255] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[256] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[257] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[258] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[259] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[260] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[261] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[262] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[263] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[264] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[265] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[266] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[267] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[268] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[269] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[270] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[271] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[272] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[273] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[274] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[275] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[276] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[277] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[278] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[279] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[280] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[281] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[282] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[283] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[284] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[285] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[286] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[287] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[288] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[289] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[290] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[291] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[292] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[293] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[294] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[295] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[296] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[297] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[298] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[299] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[300] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[301] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[302] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[303] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[304] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[305] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[306] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[307] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[308] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[309] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[310] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[311] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[312] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[313] = &Parser::MakeInstanceofExpression;
#endif

#ifndef HEADERS
    rule_action[314] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[315] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[316] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[317] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[318] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[319] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[320] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[321] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[322] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[323] = &Parser::MakeInstanceofExpression;
#endif

#ifndef HEADERS
    rule_action[324] = &Parser::MakeInstanceofExpression;
#endif

#ifndef HEADERS
    rule_action[325] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[326] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[327] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[328] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[329] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[330] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[331] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[332] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[333] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[334] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[335] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[336] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[337] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[338] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[339] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[340] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[341] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[342] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[343] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[344] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[345] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[346] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[347] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[348] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[349] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[350] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[351] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[352] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[353] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[354] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[355] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[356] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[357] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[358] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[359] = &Parser::MakeConditionalExpression;
#endif

#ifndef HEADERS
    rule_action[360] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[361] = &Parser::MakeConditionalExpression;
#endif

#ifndef HEADERS
    rule_action[362] = &Parser::MakeConditionalExpression;
#endif

#ifndef HEADERS
    rule_action[363] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[364] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[365] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[366] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[367] = &Parser::Act367;
#else
    void Act367();
#endif

#ifndef HEADERS
    rule_action[368] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[369] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[370] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[371] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[372] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[373] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[374] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[375] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[376] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[377] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[378] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[379] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[380] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[381] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[382] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[383] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[384] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[385] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[386] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[387] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[388] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[389] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[390] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[391] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[392] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[393] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[394] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[395] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[396] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[397] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[398] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[399] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[400] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[401] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[402] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[403] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[404] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[405] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[406] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[407] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[408] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[409] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[410] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[411] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[412] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[413] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[414] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[415] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[416] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[417] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[418] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[419] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[420] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[421] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[422] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[423] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[424] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[425] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[426] = &Parser::Act426;
#else
    void Act426();
#endif

#ifndef HEADERS
    rule_action[427] = &Parser::Act427;
#else
    void Act427();
#endif

#ifndef HEADERS
    rule_action[428] = &Parser::NullAction;
#endif

#ifndef HEADERS
    return;
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // ! HEADERS
