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
    AstTypeName* MakeTypeArguments(int tokennum);
    AstType* MakeArrayType(int tokennum);
    AstName* MakeSimpleName(int tokennum);
    AstModifiers* MakeModifiers();
    AstTypeParameters* MakeTypeParameters(int tokennum);
    AstArguments* MakeArguments(int tokennum);
    AstTypeArguments* MakeExplicitTypeArguments(int tokennum);
    AstBlock* MakeBlock(int tokennum);
    AstStatement* MakeSwitchBlockStatement(AstListNode* labels,
                                           AstListNode* statements = NULL);
    void MakeCastExpression(AstType* type, int tokennum);

    void BadAction();
    void NoAction();
    void NullAction();
    void SetSym1ToSym2();
    void StartList();
    void AddList2();
    void AddList3();
    void MakeTypeArguments();
    void MakeArrayType();
    void MakeCompilationUnit();
    void MakeImportDeclaration();
    void MakeModifier();
    void MakeAnnotation();
    void MakeArrayInitializer();
    void MakeClassBody();
    void MakeMethodDeclaration();
    void MakeMethodHeader();
    void MakeMethodDeclarator();
    void MakeFormalParameter();
    void MakeConstructorDeclaration();
    void MakeQualifiedSuper();
    void MakeLocalVariable();
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
    void MakeQualifiedNew();
    void MakeArrayCreationUninitialized();
    void MakeArrayCreationInitialized();
    void MakeFieldAccess();
    void MakeMethodInvocation();
    void MakeArrayAccess();
    void MakePreUnaryExpression();
    void MakeCastExpression();
    void MakeBinaryExpression();
    void MakeInstanceofExpression();
    void MakeConditionalExpression();
    void MakeWildcard();
    void MakeTypeParameter();
    void MakeTypeBound();
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
    rule_action[26] = &Parser::Act26;
#else
    void Act26();
#endif

#ifndef HEADERS
    rule_action[27] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[28] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[29] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[30] = &Parser::MakeTypeArguments;
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
    rule_action[33] = &Parser::MakeArrayType;
#endif

#ifndef HEADERS
    rule_action[34] = &Parser::MakeArrayType;
#endif

#ifndef HEADERS
    rule_action[35] = &Parser::Act35;
#else
    void Act35();
#endif

#ifndef HEADERS
    rule_action[36] = &Parser::Act36;
#else
    void Act36();
#endif

#ifndef HEADERS
    rule_action[37] = &Parser::Act37;
#else
    void Act37();
#endif

#ifndef HEADERS
    rule_action[38] = &Parser::Act38;
#else
    void Act38();
#endif

#ifndef HEADERS
    rule_action[39] = &Parser::MakeCompilationUnit;
#endif

#ifndef HEADERS
    rule_action[40] = &Parser::MakeCompilationUnit;
#endif

#ifndef HEADERS
    rule_action[41] = &Parser::Act41;
#else
    void Act41();
#endif

#ifndef HEADERS
    rule_action[42] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[43] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[44] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[45] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[46] = &Parser::Act46;
#else
    void Act46();
#endif

#ifndef HEADERS
    rule_action[47] = &Parser::NoAction;
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
    rule_action[51] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[52] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[53] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[54] = &Parser::MakeImportDeclaration;
#endif

#ifndef HEADERS
    rule_action[55] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[56] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[57] = &Parser::Act57;
#else
    void Act57();
#endif

#ifndef HEADERS
    rule_action[58] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[59] = &Parser::AddList2;
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
    rule_action[65] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[66] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[67] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[68] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[69] = &Parser::MakeModifier;
#endif

#ifndef HEADERS
    rule_action[70] = &Parser::MakeModifier;
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
    rule_action[75] = &Parser::MakeAnnotation;
#endif

#ifndef HEADERS
    rule_action[76] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[77] = &Parser::AddList3;
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
    rule_action[81] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[82] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[83] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[84] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[85] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[86] = &Parser::MakeAnnotation;
#endif

#ifndef HEADERS
    rule_action[87] = &Parser::Act87;
#else
    void Act87();
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
    rule_action[90] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[91] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[92] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[93] = &Parser::MakeClassBody;
#endif

#ifndef HEADERS
    rule_action[94] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[95] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[96] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[97] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[98] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[99] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[100] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[101] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[102] = &Parser::Act102;
#else
    void Act102();
#endif

#ifndef HEADERS
    rule_action[103] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[104] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[105] = &Parser::Act105;
#else
    void Act105();
#endif

#ifndef HEADERS
    rule_action[106] = &Parser::Act106;
#else
    void Act106();
#endif

#ifndef HEADERS
    rule_action[107] = &Parser::Act107;
#else
    void Act107();
#endif

#ifndef HEADERS
    rule_action[108] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[109] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[110] = &Parser::MakeMethodDeclaration;
#endif

#ifndef HEADERS
    rule_action[111] = &Parser::MakeMethodDeclaration;
#endif

#ifndef HEADERS
    rule_action[112] = &Parser::MakeMethodHeader;
#endif

#ifndef HEADERS
    rule_action[113] = &Parser::MakeMethodHeader;
#endif

#ifndef HEADERS
    rule_action[114] = &Parser::MakeMethodHeader;
#endif

#ifndef HEADERS
    rule_action[115] = &Parser::MakeMethodHeader;
#endif

#ifndef HEADERS
    rule_action[116] = &Parser::MakeMethodDeclarator;
#endif

#ifndef HEADERS
    rule_action[117] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[118] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[119] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[120] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[121] = &Parser::MakeFormalParameter;
#endif

#ifndef HEADERS
    rule_action[122] = &Parser::MakeFormalParameter;
#endif

#ifndef HEADERS
    rule_action[123] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[124] = &Parser::MakeFormalParameter;
#endif

#ifndef HEADERS
    rule_action[125] = &Parser::MakeFormalParameter;
#endif

#ifndef HEADERS
    rule_action[126] = &Parser::SetSym1ToSym2;
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
    rule_action[129] = &Parser::MakeConstructorDeclaration;
#endif

#ifndef HEADERS
    rule_action[130] = &Parser::MakeConstructorDeclaration;
#endif

#ifndef HEADERS
    rule_action[131] = &Parser::MakeMethodDeclarator;
#endif

#ifndef HEADERS
    rule_action[132] = &Parser::Act132;
#else
    void Act132();
#endif

#ifndef HEADERS
    rule_action[133] = &Parser::Act133;
#else
    void Act133();
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
    rule_action[136] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[137] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[138] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[139] = &Parser::Act139;
#else
    void Act139();
#endif

#ifndef HEADERS
    rule_action[140] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[141] = &Parser::MakeClassBody;
#endif

#ifndef HEADERS
    rule_action[142] = &Parser::Act142;
#else
    void Act142();
#endif

#ifndef HEADERS
    rule_action[143] = &Parser::Act143;
#else
    void Act143();
#endif

#ifndef HEADERS
    rule_action[144] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[145] = &Parser::MakeArrayInitializer;
#endif

#ifndef HEADERS
    rule_action[146] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[147] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[148] = &Parser::Act148;
#else
    void Act148();
#endif

#ifndef HEADERS
    rule_action[149] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[150] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[151] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[152] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[153] = &Parser::Act153;
#else
    void Act153();
#endif

#ifndef HEADERS
    rule_action[154] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[155] = &Parser::Act155;
#else
    void Act155();
#endif

#ifndef HEADERS
    rule_action[156] = &Parser::MakeLocalVariable;
#endif

#ifndef HEADERS
    rule_action[157] = &Parser::MakeLocalVariable;
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
    rule_action[177] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[178] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[179] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[180] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[181] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[182] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[183] = &Parser::Act183;
#else
    void Act183();
#endif

#ifndef HEADERS
    rule_action[184] = &Parser::MakeLabeledStatement;
#endif

#ifndef HEADERS
    rule_action[185] = &Parser::MakeLabeledStatement;
#endif

#ifndef HEADERS
    rule_action[186] = &Parser::Act186;
#else
    void Act186();
#endif

#ifndef HEADERS
    rule_action[187] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[188] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[189] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[190] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[191] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[192] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[193] = &Parser::MakeExpressionStatement;
#endif

#ifndef HEADERS
    rule_action[194] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[195] = &Parser::MakeIfThenElseStatement;
#endif

#ifndef HEADERS
    rule_action[196] = &Parser::MakeIfThenElseStatement;
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
    rule_action[200] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[201] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[202] = &Parser::Act202;
#else
    void Act202();
#endif

#ifndef HEADERS
    rule_action[203] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[204] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[205] = &Parser::MakeSwitchLabel;
#endif

#ifndef HEADERS
    rule_action[206] = &Parser::MakeSwitchLabel;
#endif

#ifndef HEADERS
    rule_action[207] = &Parser::MakeWhileStatement;
#endif

#ifndef HEADERS
    rule_action[208] = &Parser::MakeWhileStatement;
#endif

#ifndef HEADERS
    rule_action[209] = &Parser::Act209;
#else
    void Act209();
#endif

#ifndef HEADERS
    rule_action[210] = &Parser::MakeForStatement;
#endif

#ifndef HEADERS
    rule_action[211] = &Parser::MakeForStatement;
#endif

#ifndef HEADERS
    rule_action[212] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[213] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[214] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[215] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[216] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[217] = &Parser::MakeForeachStatement;
#endif

#ifndef HEADERS
    rule_action[218] = &Parser::MakeForeachStatement;
#endif

#ifndef HEADERS
    rule_action[219] = &Parser::MakeAssertStatement;
#endif

#ifndef HEADERS
    rule_action[220] = &Parser::MakeAssertStatement;
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
    rule_action[226] = &Parser::MakeTryStatement;
#endif

#ifndef HEADERS
    rule_action[227] = &Parser::MakeTryStatement;
#endif

#ifndef HEADERS
    rule_action[228] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[229] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[230] = &Parser::Act230;
#else
    void Act230();
#endif

#ifndef HEADERS
    rule_action[231] = &Parser::Act231;
#else
    void Act231();
#endif

#ifndef HEADERS
    rule_action[232] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[233] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[234] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[235] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[236] = &Parser::Act236;
#else
    void Act236();
#endif

#ifndef HEADERS
    rule_action[237] = &Parser::MakeParenthesizedExpression;
#endif

#ifndef HEADERS
    rule_action[238] = &Parser::MakeParenthesizedExpression;
#endif

#ifndef HEADERS
    rule_action[239] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[240] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[241] = &Parser::Act241;
#else
    void Act241();
#endif

#ifndef HEADERS
    rule_action[242] = &Parser::MakeClassLiteral;
#endif

#ifndef HEADERS
    rule_action[243] = &Parser::MakeClassLiteral;
#endif

#ifndef HEADERS
    rule_action[244] = &Parser::MakeClassLiteral;
#endif

#ifndef HEADERS
    rule_action[245] = &Parser::MakeClassLiteral;
#endif

#ifndef HEADERS
    rule_action[246] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[247] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[248] = &Parser::Act248;
#else
    void Act248();
#endif

#ifndef HEADERS
    rule_action[249] = &Parser::Act249;
#else
    void Act249();
#endif

#ifndef HEADERS
    rule_action[250] = &Parser::MakeQualifiedNew;
#endif

#ifndef HEADERS
    rule_action[251] = &Parser::MakeQualifiedNew;
#endif

#ifndef HEADERS
    rule_action[252] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[253] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[254] = &Parser::MakeArrayCreationUninitialized;
#endif

#ifndef HEADERS
    rule_action[255] = &Parser::MakeArrayCreationUninitialized;
#endif

#ifndef HEADERS
    rule_action[256] = &Parser::MakeArrayCreationInitialized;
#endif

#ifndef HEADERS
    rule_action[257] = &Parser::MakeArrayCreationInitialized;
#endif

#ifndef HEADERS
    rule_action[258] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[259] = &Parser::AddList2;
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
    rule_action[263] = &Parser::Act263;
#else
    void Act263();
#endif

#ifndef HEADERS
    rule_action[264] = &Parser::Act264;
#else
    void Act264();
#endif

#ifndef HEADERS
    rule_action[265] = &Parser::MakeFieldAccess;
#endif

#ifndef HEADERS
    rule_action[266] = &Parser::MakeFieldAccess;
#endif

#ifndef HEADERS
    rule_action[267] = &Parser::Act267;
#else
    void Act267();
#endif

#ifndef HEADERS
    rule_action[268] = &Parser::MakeMethodInvocation;
#endif

#ifndef HEADERS
    rule_action[269] = &Parser::MakeMethodInvocation;
#endif

#ifndef HEADERS
    rule_action[270] = &Parser::MakeMethodInvocation;
#endif

#ifndef HEADERS
    rule_action[271] = &Parser::MakeMethodInvocation;
#endif

#ifndef HEADERS
    rule_action[272] = &Parser::MakeMethodInvocation;
#endif

#ifndef HEADERS
    rule_action[273] = &Parser::MakeMethodInvocation;
#endif

#ifndef HEADERS
    rule_action[274] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[275] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[276] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[277] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[278] = &Parser::NoAction;
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
    rule_action[282] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[283] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[284] = &Parser::Act284;
#else
    void Act284();
#endif

#ifndef HEADERS
    rule_action[285] = &Parser::Act285;
#else
    void Act285();
#endif

#ifndef HEADERS
    rule_action[286] = &Parser::NoAction;
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
    rule_action[292] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[293] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[294] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[295] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[296] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[297] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[298] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[299] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[300] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[301] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[302] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[303] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[304] = &Parser::MakePreUnaryExpression;
#endif

#ifndef HEADERS
    rule_action[305] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[306] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[307] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[308] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[309] = &Parser::Act309;
#else
    void Act309();
#endif

#ifndef HEADERS
    rule_action[310] = &Parser::Act310;
#else
    void Act310();
#endif

#ifndef HEADERS
    rule_action[311] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[312] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[313] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[314] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[315] = &Parser::NoAction;
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
    rule_action[322] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[323] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[324] = &Parser::MakeBinaryExpression;
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
    rule_action[328] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[329] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[330] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[331] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[332] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[333] = &Parser::MakeBinaryExpression;
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
    rule_action[341] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[342] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[343] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[344] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[345] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[346] = &Parser::MakeInstanceofExpression;
#endif

#ifndef HEADERS
    rule_action[347] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[348] = &Parser::MakeBinaryExpression;
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
    rule_action[353] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[354] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[355] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[356] = &Parser::MakeInstanceofExpression;
#endif

#ifndef HEADERS
    rule_action[357] = &Parser::MakeInstanceofExpression;
#endif

#ifndef HEADERS
    rule_action[358] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[359] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[360] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[361] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[362] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[363] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[364] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[365] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[366] = &Parser::NoAction;
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
    rule_action[370] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[371] = &Parser::NoAction;
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
    rule_action[375] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[376] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[377] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[378] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[379] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[380] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[381] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[382] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[383] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[384] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[385] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[386] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[387] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[388] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[389] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[390] = &Parser::MakeBinaryExpression;
#endif

#ifndef HEADERS
    rule_action[391] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[392] = &Parser::MakeConditionalExpression;
#endif

#ifndef HEADERS
    rule_action[393] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[394] = &Parser::MakeConditionalExpression;
#endif

#ifndef HEADERS
    rule_action[395] = &Parser::MakeConditionalExpression;
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
    rule_action[400] = &Parser::Act400;
#else
    void Act400();
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
    rule_action[406] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[407] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[408] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[409] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[410] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[411] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[412] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[413] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[414] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[415] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[416] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[417] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[418] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[419] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[420] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[421] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[422] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[423] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[424] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[425] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[426] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[427] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[428] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[429] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[430] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[431] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[432] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[433] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[434] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[435] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[436] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[437] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[438] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[439] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[440] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[441] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[442] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[443] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[444] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[445] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[446] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[447] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[448] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[449] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[450] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[451] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[452] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[453] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[454] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[455] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[456] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[457] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[458] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[459] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[460] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[461] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[462] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[463] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[464] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[465] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[466] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[467] = &Parser::Act467;
#else
    void Act467();
#endif

#ifndef HEADERS
    rule_action[468] = &Parser::Act468;
#else
    void Act468();
#endif

#ifndef HEADERS
    rule_action[469] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[470] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[471] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[472] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[473] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[474] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[475] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[476] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[477] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[478] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[479] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[480] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[481] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[482] = &Parser::MakeWildcard;
#endif

#ifndef HEADERS
    rule_action[483] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[484] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[485] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[486] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[487] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[488] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[489] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[490] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[491] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[492] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[493] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[494] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[495] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[496] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[497] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[498] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[499] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[500] = &Parser::MakeTypeArguments;
#endif

#ifndef HEADERS
    rule_action[501] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[502] = &Parser::MakeTypeArguments;
#endif

#ifndef HEADERS
    rule_action[503] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[504] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[505] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[506] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[507] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[508] = &Parser::AddList3;
#endif

#ifndef HEADERS
    rule_action[509] = &Parser::MakeTypeParameter;
#endif

#ifndef HEADERS
    rule_action[510] = &Parser::MakeTypeParameter;
#endif

#ifndef HEADERS
    rule_action[511] = &Parser::MakeTypeParameter;
#endif

#ifndef HEADERS
    rule_action[512] = &Parser::MakeTypeBound;
#endif

#ifndef HEADERS
    rule_action[513] = &Parser::MakeTypeBound;
#endif

#ifndef HEADERS
    rule_action[514] = &Parser::MakeTypeBound;
#endif

#ifndef HEADERS
    rule_action[515] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[516] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[517] = &Parser::StartList;
#endif

#ifndef HEADERS
    rule_action[518] = &Parser::AddList2;
#endif

#ifndef HEADERS
    rule_action[519] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[520] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[521] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[522] = &Parser::MakeTypeArguments;
#endif

#ifndef HEADERS
    return;
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // ! HEADERS
