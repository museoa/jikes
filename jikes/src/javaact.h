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
    AstStatement* MakeSwitchBlockStatement(AstListNode* labels,
                                           AstListNode* statements = NULL);

    void BadAction();
    void NoAction();
    void NullAction();
    void MakeArrayType();
    void MakeSimpleName();
    void MakeFieldAccess();
    void MakeQualifiedSuper();
    void MakeQualifiedNew();
    void SetSym1ToSym2();
    void MakeMethodDeclaration();
    void MakeLabeledStatement();
    void MakeExpressionStatement();
    void MakeIfThenElseStatement();
    void MakeSwitchLabel();
    void MakeWhileStatement();
    void MakeForStatement();
    void MakeAssertStatement();
    void MakeArrayCreationExpression();
    void MakeSuperFieldAccess();
    void MakeSuperDoubleFieldAccess();
    void MakeArrayAccess();
    void MakeCastExpression();
#endif // HEADERS

#ifndef HEADERS
    rule_action[1] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[2] = &Parser::Act2;
#else
    void Act2();
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
    rule_action[28] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[29] = &Parser::MakeArrayType;
#endif

#ifndef HEADERS
    rule_action[30] = &Parser::MakeArrayType;
#endif

#ifndef HEADERS
    rule_action[31] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[32] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[33] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[34] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[35] = &Parser::MakeSimpleName;
#endif

#ifndef HEADERS
    rule_action[36] = &Parser::MakeFieldAccess;
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
    rule_action[39] = &Parser::Act39;
#else
    void Act39();
#endif

#ifndef HEADERS
    rule_action[40] = &Parser::Act40;
#else
    void Act40();
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
    rule_action[52] = &Parser::Act52;
#else
    void Act52();
#endif

#ifndef HEADERS
    rule_action[53] = &Parser::Act53;
#else
    void Act53();
#endif

#ifndef HEADERS
    rule_action[54] = &Parser::Act54;
#else
    void Act54();
#endif

#ifndef HEADERS
    rule_action[55] = &Parser::Act55;
#else
    void Act55();
#endif

#ifndef HEADERS
    rule_action[56] = &Parser::Act56;
#else
    void Act56();
#endif

#ifndef HEADERS
    rule_action[57] = &Parser::Act57;
#else
    void Act57();
#endif

#ifndef HEADERS
    rule_action[58] = &Parser::Act58;
#else
    void Act58();
#endif

#ifndef HEADERS
    rule_action[59] = &Parser::Act59;
#else
    void Act59();
#endif

#ifndef HEADERS
    rule_action[60] = &Parser::Act60;
#else
    void Act60();
#endif

#ifndef HEADERS
    rule_action[61] = &Parser::Act61;
#else
    void Act61();
#endif

#ifndef HEADERS
    rule_action[62] = &Parser::Act62;
#else
    void Act62();
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
    rule_action[66] = &Parser::Act66;
#else
    void Act66();
#endif

#ifndef HEADERS
    rule_action[67] = &Parser::Act67;
#else
    void Act67();
#endif

#ifndef HEADERS
    rule_action[68] = &Parser::Act68;
#else
    void Act68();
#endif

#ifndef HEADERS
    rule_action[69] = &Parser::Act69;
#else
    void Act69();
#endif

#ifndef HEADERS
    rule_action[70] = &Parser::Act70;
#else
    void Act70();
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
    rule_action[74] = &Parser::Act74;
#else
    void Act74();
#endif

#ifndef HEADERS
    rule_action[75] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[76] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[77] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[78] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[79] = &Parser::Act79;
#else
    void Act79();
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
    rule_action[83] = &Parser::Act83;
#else
    void Act83();
#endif

#ifndef HEADERS
    rule_action[84] = &Parser::Act84;
#else
    void Act84();
#endif

#ifndef HEADERS
    rule_action[85] = &Parser::Act85;
#else
    void Act85();
#endif

#ifndef HEADERS
    rule_action[86] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[87] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[88] = &Parser::MakeMethodDeclaration;
#endif

#ifndef HEADERS
    rule_action[89] = &Parser::MakeMethodDeclaration;
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
    rule_action[92] = &Parser::Act92;
#else
    void Act92();
#endif

#ifndef HEADERS
    rule_action[93] = &Parser::Act93;
#else
    void Act93();
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
    rule_action[96] = &Parser::Act96;
#else
    void Act96();
#endif

#ifndef HEADERS
    rule_action[97] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[98] = &Parser::Act98;
#else
    void Act98();
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
    rule_action[102] = &Parser::Act102;
#else
    void Act102();
#endif

#ifndef HEADERS
    rule_action[103] = &Parser::Act103;
#else
    void Act103();
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
    rule_action[106] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[107] = &Parser::MakeQualifiedSuper;
#endif

#ifndef HEADERS
    rule_action[108] = &Parser::Act108;
#else
    void Act108();
#endif

#ifndef HEADERS
    rule_action[109] = &Parser::SetSym1ToSym2;
#endif

#ifndef HEADERS
    rule_action[110] = &Parser::Act110;
#else
    void Act110();
#endif

#ifndef HEADERS
    rule_action[111] = &Parser::Act111;
#else
    void Act111();
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
    rule_action[114] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[115] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[116] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[117] = &Parser::Act117;
#else
    void Act117();
#endif

#ifndef HEADERS
    rule_action[118] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[119] = &Parser::Act119;
#else
    void Act119();
#endif

#ifndef HEADERS
    rule_action[120] = &Parser::Act120;
#else
    void Act120();
#endif

#ifndef HEADERS
    rule_action[121] = &Parser::Act121;
#else
    void Act121();
#endif

#ifndef HEADERS
    rule_action[122] = &Parser::Act122;
#else
    void Act122();
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
    rule_action[125] = &Parser::Act125;
#else
    void Act125();
#endif

#ifndef HEADERS
    rule_action[126] = &Parser::Act126;
#else
    void Act126();
#endif

#ifndef HEADERS
    rule_action[127] = &Parser::Act127;
#else
    void Act127();
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
    rule_action[169] = &Parser::Act169;
#else
    void Act169();
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
    rule_action[175] = &Parser::Act175;
#else
    void Act175();
#endif

#ifndef HEADERS
    rule_action[176] = &Parser::Act176;
#else
    void Act176();
#endif

#ifndef HEADERS
    rule_action[177] = &Parser::Act177;
#else
    void Act177();
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
    rule_action[188] = &Parser::Act188;
#else
    void Act188();
#endif

#ifndef HEADERS
    rule_action[189] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[190] = &Parser::Act190;
#else
    void Act190();
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
    rule_action[194] = &Parser::Act194;
#else
    void Act194();
#endif

#ifndef HEADERS
    rule_action[195] = &Parser::Act195;
#else
    void Act195();
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
    rule_action[201] = &Parser::Act201;
#else
    void Act201();
#endif

#ifndef HEADERS
    rule_action[202] = &Parser::Act202;
#else
    void Act202();
#endif

#ifndef HEADERS
    rule_action[203] = &Parser::Act203;
#else
    void Act203();
#endif

#ifndef HEADERS
    rule_action[204] = &Parser::Act204;
#else
    void Act204();
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
    rule_action[212] = &Parser::Act212;
#else
    void Act212();
#endif

#ifndef HEADERS
    rule_action[213] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[214] = &Parser::NoAction;
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
    rule_action[218] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[219] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[220] = &Parser::Act220;
#else
    void Act220();
#endif

#ifndef HEADERS
    rule_action[221] = &Parser::MakeQualifiedNew;
#endif

#ifndef HEADERS
    rule_action[222] = &Parser::MakeQualifiedNew;
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
    rule_action[225] = &Parser::MakeArrayCreationExpression;
#endif

#ifndef HEADERS
    rule_action[226] = &Parser::MakeArrayCreationExpression;
#endif

#ifndef HEADERS
    rule_action[227] = &Parser::Act227;
#else
    void Act227();
#endif

#ifndef HEADERS
    rule_action[228] = &Parser::Act228;
#else
    void Act228();
#endif

#ifndef HEADERS
    rule_action[229] = &Parser::Act229;
#else
    void Act229();
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
    rule_action[232] = &Parser::Act232;
#else
    void Act232();
#endif

#ifndef HEADERS
    rule_action[233] = &Parser::MakeFieldAccess;
#endif

#ifndef HEADERS
    rule_action[234] = &Parser::MakeSuperFieldAccess;
#endif

#ifndef HEADERS
    rule_action[235] = &Parser::MakeSuperDoubleFieldAccess;
#endif

#ifndef HEADERS
    rule_action[236] = &Parser::Act236;
#else
    void Act236();
#endif

#ifndef HEADERS
    rule_action[237] = &Parser::Act237;
#else
    void Act237();
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
    rule_action[240] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[241] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[242] = &Parser::MakeArrayAccess;
#endif

#ifndef HEADERS
    rule_action[243] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[244] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[245] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[246] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[247] = &Parser::Act247;
#else
    void Act247();
#endif

#ifndef HEADERS
    rule_action[248] = &Parser::Act248;
#else
    void Act248();
#endif

#ifndef HEADERS
    rule_action[249] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[250] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[251] = &Parser::Act251;
#else
    void Act251();
#endif

#ifndef HEADERS
    rule_action[252] = &Parser::Act252;
#else
    void Act252();
#endif

#ifndef HEADERS
    rule_action[253] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[254] = &Parser::Act254;
#else
    void Act254();
#endif

#ifndef HEADERS
    rule_action[255] = &Parser::Act255;
#else
    void Act255();
#endif

#ifndef HEADERS
    rule_action[256] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[257] = &Parser::Act257;
#else
    void Act257();
#endif

#ifndef HEADERS
    rule_action[258] = &Parser::Act258;
#else
    void Act258();
#endif

#ifndef HEADERS
    rule_action[259] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[260] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[261] = &Parser::Act261;
#else
    void Act261();
#endif

#ifndef HEADERS
    rule_action[262] = &Parser::MakeCastExpression;
#endif

#ifndef HEADERS
    rule_action[263] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[264] = &Parser::Act264;
#else
    void Act264();
#endif

#ifndef HEADERS
    rule_action[265] = &Parser::Act265;
#else
    void Act265();
#endif

#ifndef HEADERS
    rule_action[266] = &Parser::Act266;
#else
    void Act266();
#endif

#ifndef HEADERS
    rule_action[267] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[268] = &Parser::Act268;
#else
    void Act268();
#endif

#ifndef HEADERS
    rule_action[269] = &Parser::Act269;
#else
    void Act269();
#endif

#ifndef HEADERS
    rule_action[270] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[271] = &Parser::Act271;
#else
    void Act271();
#endif

#ifndef HEADERS
    rule_action[272] = &Parser::Act272;
#else
    void Act272();
#endif

#ifndef HEADERS
    rule_action[273] = &Parser::Act273;
#else
    void Act273();
#endif

#ifndef HEADERS
    rule_action[274] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[275] = &Parser::Act275;
#else
    void Act275();
#endif

#ifndef HEADERS
    rule_action[276] = &Parser::Act276;
#else
    void Act276();
#endif

#ifndef HEADERS
    rule_action[277] = &Parser::Act277;
#else
    void Act277();
#endif

#ifndef HEADERS
    rule_action[278] = &Parser::Act278;
#else
    void Act278();
#endif

#ifndef HEADERS
    rule_action[279] = &Parser::Act279;
#else
    void Act279();
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
    rule_action[282] = &Parser::Act282;
#else
    void Act282();
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
    rule_action[285] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[286] = &Parser::Act286;
#else
    void Act286();
#endif

#ifndef HEADERS
    rule_action[287] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[288] = &Parser::Act288;
#else
    void Act288();
#endif

#ifndef HEADERS
    rule_action[289] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[290] = &Parser::Act290;
#else
    void Act290();
#endif

#ifndef HEADERS
    rule_action[291] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[292] = &Parser::Act292;
#else
    void Act292();
#endif

#ifndef HEADERS
    rule_action[293] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[294] = &Parser::Act294;
#else
    void Act294();
#endif

#ifndef HEADERS
    rule_action[295] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[296] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[297] = &Parser::Act297;
#else
    void Act297();
#endif

#ifndef HEADERS
    rule_action[298] = &Parser::Act298;
#else
    void Act298();
#endif

#ifndef HEADERS
    rule_action[299] = &Parser::Act299;
#else
    void Act299();
#endif

#ifndef HEADERS
    rule_action[300] = &Parser::Act300;
#else
    void Act300();
#endif

#ifndef HEADERS
    rule_action[301] = &Parser::Act301;
#else
    void Act301();
#endif

#ifndef HEADERS
    rule_action[302] = &Parser::Act302;
#else
    void Act302();
#endif

#ifndef HEADERS
    rule_action[303] = &Parser::Act303;
#else
    void Act303();
#endif

#ifndef HEADERS
    rule_action[304] = &Parser::Act304;
#else
    void Act304();
#endif

#ifndef HEADERS
    rule_action[305] = &Parser::Act305;
#else
    void Act305();
#endif

#ifndef HEADERS
    rule_action[306] = &Parser::Act306;
#else
    void Act306();
#endif

#ifndef HEADERS
    rule_action[307] = &Parser::Act307;
#else
    void Act307();
#endif

#ifndef HEADERS
    rule_action[308] = &Parser::Act308;
#else
    void Act308();
#endif

#ifndef HEADERS
    rule_action[309] = &Parser::Act309;
#else
    void Act309();
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
    rule_action[339] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[340] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[341] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[342] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[343] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[344] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[345] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[346] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[347] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[348] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[349] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[350] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[351] = &Parser::NullAction;
#endif

#ifndef HEADERS
    rule_action[352] = &Parser::NoAction;
#endif

#ifndef HEADERS
    rule_action[353] = &Parser::Act353;
#else
    void Act353();
#endif

#ifndef HEADERS
    rule_action[354] = &Parser::Act354;
#else
    void Act354();
#endif

#ifndef HEADERS
    rule_action[355] = &Parser::NullAction;
#endif

#ifndef HEADERS
    return;
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // ! HEADERS
