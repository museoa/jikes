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
    AstModifiers* MakeModifiers();
    void MakeFormalParameter(AstModifiers* modifiers, AstType* type,
                             int ellipsis_token,
                             AstVariableDeclaratorId* variable);
    AstArguments* MakeArguments(int tokennum);
    void MakeLocalVariable(AstModifiers* modifiers, AstType* type,
                           AstListNode* variables);
    AstBlock* MakeBlock(int tokennum);
    AstStatement* MakeSwitchBlockStatement(AstListNode* labels,
                                           AstListNode* statements = NULL);
    AstMethodInvocation* MakeMethodInvocation(int tokennum);
    void MakeCastExpression(AstType* type, int tokennum);

    void BadAction();
    void NoAction();
    void NullAction();
    void SetSym1ToSym2();
    void StartList();
    void AddList2();
    void AddList3();
    void MakeArrayType();
    void MakeCompilationUnit();
    void MakeImportDeclaration();
    void MakeModifier();
    void MakeAnnotation();
    void MakeArrayInitializer();
    void MakeClassBody();
    void MakeQualifiedSuper();
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
    rule_action[33] = &Parser::MakeCompilationUnit;
#endif

#ifndef HEADERS
    rule_action[34] = &Parser::MakeCompilationUnit;
#endif

#ifndef HEADERS
    rule_action[35] = &Parser::Act35;
#else
    void Act35();
#endif

#ifndef HEADERS
    rule_action[36] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[37] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[38] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[39] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[40] = &Parser::Act40;
#else
    void Act40();
#endif

#ifndef HEADERS
    rule_action[41] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[42] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[43] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[44] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[45] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[46] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[47] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[48] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[49] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[50] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[51] = &Parser::Act51;
#else
    void Act51();
#endif

#ifndef HEADERS
    rule_action[52] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[53] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[54] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[55] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[56] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[57] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[58] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[59] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[60] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[61] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[62] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[63] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[64] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[65] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[66] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[67] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[68] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[69] = &Parser::MakeAnnotation;
#endif

#ifndef HEADERS
    rule_action[70] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[71] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[72] = &Parser::Act72;
#else
    void Act72();
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
    rule_action[76] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[77] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[78] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[79] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[80] = &Parser::MakeAnnotation;
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
    rule_action[83] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[84] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[85] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[86] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[87] = &Parser::MakeClassBody;
#endif

#ifndef HEADERS
    rule_action[88] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[89] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[90] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[91] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[92] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[93] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[94] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[95] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[96] = &Parser::Act96;
#else
    void Act96();
#endif

#ifndef HEADERS
    rule_action[97] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[98] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[99] = &Parser::Act99;
#else
    void Act99();
#endif

#ifndef HEADERS
    rule_action[100] = &Parser::Act100;
#else
    void Act100();
#endif

#ifndef HEADERS
    rule_action[101] = &Parser::Act101;
#else
    void Act101();
#endif

#ifndef HEADERS
    rule_action[102] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[103] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[104] = &Parser::MakeMethodDeclaration;
#endif

#ifndef HEADERS
    rule_action[105] = &Parser::MakeMethodDeclaration;
#endif

#ifndef HEADERS
    rule_action[106] = &Parser::MakeMethodHeader;
#endif

#ifndef HEADERS
    rule_action[107] = &Parser::Act107;
#else
    void Act107();
#endif

#ifndef HEADERS
    rule_action[108] = &Parser::MakeMethodDeclarator;
#endif

#ifndef HEADERS
    rule_action[109] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[110] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[111] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[112] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[113] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[114] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[115] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[116] = &Parser::MakeFormalParameterB;
#endif

#ifndef HEADERS
    rule_action[117] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[118] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[119] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[120] = &Parser::MakeFormalParameterA;
#endif

#ifndef HEADERS
    rule_action[121] = &Parser::MakeFormalParameterB;
#endif

#ifndef HEADERS
    rule_action[122] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[123] = &Parser::Act123;
#else
    void Act123();
#endif

#ifndef HEADERS
    rule_action[124] = &Parser::Act124;
#else
    void Act124();
#endif

#ifndef HEADERS
    rule_action[125] = &Parser::MakeConstructorDeclaration;
#endif

#ifndef HEADERS
    rule_action[126] = &Parser::MakeMethodDeclarator;
#endif

#ifndef HEADERS
    rule_action[127] = &Parser::Act127;
#else
    void Act127();
#endif

#ifndef HEADERS
    rule_action[128] = &Parser::Act128;
#else
    void Act128();
#endif

#ifndef HEADERS
    rule_action[129] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[130] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[131] = &Parser::Act131;
#else
    void Act131();
#endif

#ifndef HEADERS
    rule_action[132] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[133] = &Parser::MakeClassBody;
#endif

#ifndef HEADERS
    rule_action[134] = &Parser::Act134;
#else
    void Act134();
#endif

#ifndef HEADERS
    rule_action[135] = &Parser::Act135;
#else
    void Act135();
#endif

#ifndef HEADERS
    rule_action[136] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[137] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[138] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[139] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[140] = &Parser::Act140;
#else
    void Act140();
#endif

#ifndef HEADERS
    rule_action[141] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[142] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[143] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[144] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[145] = &Parser::Act145;
#else
    void Act145();
#endif

#ifndef HEADERS
    rule_action[146] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[147] = &Parser::Act147;
#else
    void Act147();
#endif

#ifndef HEADERS
    rule_action[148] = &Parser::MakeLocalVariable;
#endif

#ifndef HEADERS
    rule_action[149] = &Parser::MakeLocalVariable;
#endif

#ifndef HEADERS
    rule_action[150] = &Parser::MakeLocalVariable;
#endif

#ifndef HEADERS
    rule_action[151] = &Parser::Act151;
#else
    void Act151();
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
    rule_action[158] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[159] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[160] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[161] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[162] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[163] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[164] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[165] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[166] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[167] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[168] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[169] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[170] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[171] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[172] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[173] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[174] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[175] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[176] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[177] = &Parser::Act177;
#else
    void Act177();
#endif

#ifndef HEADERS
    rule_action[178] = &Parser::MakeLabeledStatement;
#endif

#ifndef HEADERS
    rule_action[179] = &Parser::MakeLabeledStatement;
#endif

#ifndef HEADERS
    rule_action[180] = &Parser::Act180;
#else
    void Act180();
#endif

#ifndef HEADERS
    rule_action[181] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[182] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[183] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[184] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[185] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[186] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[187] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[188] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[189] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[190] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[191] = &Parser::Act191;
#else
    void Act191();
#endif

#ifndef HEADERS
    rule_action[192] = &Parser::Act192;
#else
    void Act192();
#endif

#ifndef HEADERS
    rule_action[193] = &Parser::Act193;
#else
    void Act193();
#endif

#ifndef HEADERS
    rule_action[194] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[195] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[196] = &Parser::Act196;
#else
    void Act196();
#endif

#ifndef HEADERS
    rule_action[197] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[198] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[199] = &Parser::MakeSwitchLabel;
#endif

#ifndef HEADERS
    rule_action[200] = &Parser::MakeSwitchLabel;
#endif

#ifndef HEADERS
    rule_action[201] = &Parser::MakeWhileStatement;
#endif

#ifndef HEADERS
    rule_action[202] = &Parser::MakeWhileStatement;
#endif

#ifndef HEADERS
    rule_action[203] = &Parser::Act203;
#else
    void Act203();
#endif

#ifndef HEADERS
    rule_action[204] = &Parser::MakeForStatement;
#endif

#ifndef HEADERS
    rule_action[205] = &Parser::MakeForStatement;
#endif

#ifndef HEADERS
    rule_action[206] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[207] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[208] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[209] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[210] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[211] = &Parser::MakeForeachStatement;
#endif

#ifndef HEADERS
    rule_action[212] = &Parser::MakeForeachStatement;
#endif

#ifndef HEADERS
    rule_action[213] = &Parser::MakeAssertStatement;
#endif

#ifndef HEADERS
    rule_action[214] = &Parser::MakeAssertStatement;
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
    rule_action[217] = &Parser::Act217;
#else
    void Act217();
#endif

#ifndef HEADERS
    rule_action[218] = &Parser::Act218;
#else
    void Act218();
#endif

#ifndef HEADERS
    rule_action[219] = &Parser::Act219;
#else
    void Act219();
#endif

#ifndef HEADERS
    rule_action[220] = &Parser::MakeTryStatement;
#endif

#ifndef HEADERS
    rule_action[221] = &Parser::MakeTryStatement;
#endif

#ifndef HEADERS
    rule_action[222] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[223] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[224] = &Parser::Act224;
#else
    void Act224();
#endif

#ifndef HEADERS
    rule_action[225] = &Parser::Act225;
#else
    void Act225();
#endif

#ifndef HEADERS
    rule_action[226] = &Parser::NoAction;
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
    rule_action[230] = &Parser::Act230;
#else
    void Act230();
#endif

#ifndef HEADERS
    rule_action[231] = &Parser::MakeParenthesizedExpression;
#endif

#ifndef HEADERS
    rule_action[232] = &Parser::MakeParenthesizedExpression;
#endif

#ifndef HEADERS
    rule_action[233] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[234] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[235] = &Parser::Act235;
#else
    void Act235();
#endif

#ifndef HEADERS
    rule_action[236] = &Parser::MakeClassLiteral;
#endif

#ifndef HEADERS
    rule_action[237] = &Parser::MakeClassLiteral;
#endif

#ifndef HEADERS
    rule_action[238] = &Parser::Act238;
#else
    void Act238();
#endif

#ifndef HEADERS
    rule_action[239] = &Parser::Act239;
#else
    void Act239();
#endif

#ifndef HEADERS
    rule_action[240] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[241] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[242] = &Parser::Act242;
#else
    void Act242();
#endif

#ifndef HEADERS
    rule_action[243] = &Parser::MakeQualifiedNew;
#endif

#ifndef HEADERS
    rule_action[244] = &Parser::MakeQualifiedNew;
#endif

#ifndef HEADERS
    rule_action[245] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[246] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[247] = &Parser::MakeArrayCreationUninitialized;
#endif

#ifndef HEADERS
    rule_action[248] = &Parser::MakeArrayCreationUninitialized;
#endif

#ifndef HEADERS
    rule_action[249] = &Parser::Act249;
#else
    void Act249();
#endif

#ifndef HEADERS
    rule_action[250] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[251] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[252] = &Parser::Act252;
#else
    void Act252();
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
    rule_action[255] = &Parser::MakeFieldAccess;
#endif

#ifndef HEADERS
    rule_action[256] = &Parser::MakeSuperFieldAccess;
#endif

#ifndef HEADERS
    rule_action[257] = &Parser::MakeQualifiedSuperFieldAccess;
#endif

#ifndef HEADERS
    rule_action[258] = &Parser::Act258;
#else
    void Act258();
#endif

#ifndef HEADERS
    rule_action[259] = &Parser::Act259;
#else
    void Act259();
#endif

#ifndef HEADERS
    rule_action[260] = &Parser::Act260;
#else
    void Act260();
#endif

#ifndef HEADERS
    rule_action[261] = &Parser::Act261;
#else
    void Act261();
#endif

#ifndef HEADERS
    rule_action[262] = &Parser::Act262;
#else
    void Act262();
#endif

#ifndef HEADERS
    rule_action[263] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[264] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[265] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[266] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[267] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[268] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[269] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[270] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[271] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[272] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[273] = &Parser::Act273;
#else
    void Act273();
#endif

#ifndef HEADERS
    rule_action[274] = &Parser::Act274;
#else
    void Act274();
#endif

#ifndef HEADERS
    rule_action[275] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[276] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[277] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[278] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[279] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[280] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[281] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[282] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[283] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[284] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[285] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[286] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[287] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[288] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[289] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[290] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[291] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[292] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[293] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[294] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[295] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[296] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[297] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[298] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[299] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[300] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[301] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[302] = &Parser::NoAction;
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
    rule_action[308] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[309] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[310] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[311] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[312] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[313] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[314] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[315] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[316] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[317] = &Parser::NoAction;
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
    rule_action[321] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[322] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[323] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[324] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[325] = &Parser::MakeBinaryExpression;
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
    rule_action[333] = &Parser::MakeInstanceofExpression;
#endif

#ifndef HEADERS
    rule_action[334] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[335] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[336] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[337] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[338] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[339] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[340] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[341] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[342] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[343] = &Parser::MakeInstanceofExpression;
#endif

#ifndef HEADERS
    rule_action[344] = &Parser::MakeInstanceofExpression;
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
    rule_action[350] = &Parser::MakeBinaryExpression;
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
    rule_action[359] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[360] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[361] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[362] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[363] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[364] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[365] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[366] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[367] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[368] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[369] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[370] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[371] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[372] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[373] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[374] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[375] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[376] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[377] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[378] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[379] = &Parser::MakeConditionalExpression;
#endif

#ifndef HEADERS
    rule_action[380] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[381] = &Parser::MakeConditionalExpression;
#endif

#ifndef HEADERS
    rule_action[382] = &Parser::MakeConditionalExpression;
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
    rule_action[386] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[387] = &Parser::Act387;
#else
    void Act387();
#endif

#ifndef HEADERS
    rule_action[388] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[389] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[390] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[391] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[392] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[393] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[394] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[395] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[396] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[397] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[398] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[399] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[400] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[401] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[402] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[403] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[404] = &Parser::NoAction;
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
    rule_action[426] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[427] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[428] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[429] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[430] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[431] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[432] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[433] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[434] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[435] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[436] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[437] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[438] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[439] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[440] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[441] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[442] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[443] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[444] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[445] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[446] = &Parser::Act446;
#else
    void Act446();
#endif

#ifndef HEADERS
    rule_action[447] = &Parser::Act447;
#else
    void Act447();
#endif

#ifndef HEADERS
    rule_action[448] = &Parser::NullAction;
#endif

#ifndef HEADERS
    return;
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // ! HEADERS
