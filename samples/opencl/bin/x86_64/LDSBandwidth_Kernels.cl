__kernel void LDSBandwidth_single(__local DATATYPE *sharedMemory, __global DATATYPE *output)
{
DATATYPE val1 = (DATATYPE)(0);
DATATYPE val2 = val1;
uint gid = get_global_id(0);
uint lid = get_local_id(0);
val1 = val1 + sharedMemory[0];
val2 = val2 + sharedMemory[1];
val1 = val1 + sharedMemory[2];
val2 = val2 + sharedMemory[3];
val1 = val1 + sharedMemory[4];
val2 = val2 + sharedMemory[5];
val1 = val1 + sharedMemory[6];
val2 = val2 + sharedMemory[7];
val1 = val1 + sharedMemory[8];
val2 = val2 + sharedMemory[9];
val1 = val1 + sharedMemory[10];
val2 = val2 + sharedMemory[11];
val1 = val1 + sharedMemory[12];
val2 = val2 + sharedMemory[13];
val1 = val1 + sharedMemory[14];
val2 = val2 + sharedMemory[15];
val1 = val1 + sharedMemory[16];
val2 = val2 + sharedMemory[17];
val1 = val1 + sharedMemory[18];
val2 = val2 + sharedMemory[19];
val1 = val1 + sharedMemory[20];
val2 = val2 + sharedMemory[21];
val1 = val1 + sharedMemory[22];
val2 = val2 + sharedMemory[23];
val1 = val1 + sharedMemory[24];
val2 = val2 + sharedMemory[25];
val1 = val1 + sharedMemory[26];
val2 = val2 + sharedMemory[27];
val1 = val1 + sharedMemory[28];
val2 = val2 + sharedMemory[29];
val1 = val1 + sharedMemory[30];
val2 = val2 + sharedMemory[31];
val1 = val1 + sharedMemory[32];
val2 = val2 + sharedMemory[33];
val1 = val1 + sharedMemory[34];
val2 = val2 + sharedMemory[35];
val1 = val1 + sharedMemory[36];
val2 = val2 + sharedMemory[37];
val1 = val1 + sharedMemory[38];
val2 = val2 + sharedMemory[39];
val1 = val1 + sharedMemory[40];
val2 = val2 + sharedMemory[41];
val1 = val1 + sharedMemory[42];
val2 = val2 + sharedMemory[43];
val1 = val1 + sharedMemory[44];
val2 = val2 + sharedMemory[45];
val1 = val1 + sharedMemory[46];
val2 = val2 + sharedMemory[47];
val1 = val1 + sharedMemory[48];
val2 = val2 + sharedMemory[49];
val1 = val1 + sharedMemory[50];
val2 = val2 + sharedMemory[51];
val1 = val1 + sharedMemory[52];
val2 = val2 + sharedMemory[53];
val1 = val1 + sharedMemory[54];
val2 = val2 + sharedMemory[55];
val1 = val1 + sharedMemory[56];
val2 = val2 + sharedMemory[57];
val1 = val1 + sharedMemory[58];
val2 = val2 + sharedMemory[59];
val1 = val1 + sharedMemory[60];
val2 = val2 + sharedMemory[61];
val1 = val1 + sharedMemory[62];
val2 = val2 + sharedMemory[63];
val1 = val1 + sharedMemory[64];
val2 = val2 + sharedMemory[65];
val1 = val1 + sharedMemory[66];
val2 = val2 + sharedMemory[67];
val1 = val1 + sharedMemory[68];
val2 = val2 + sharedMemory[69];
val1 = val1 + sharedMemory[70];
val2 = val2 + sharedMemory[71];
val1 = val1 + sharedMemory[72];
val2 = val2 + sharedMemory[73];
val1 = val1 + sharedMemory[74];
val2 = val2 + sharedMemory[75];
val1 = val1 + sharedMemory[76];
val2 = val2 + sharedMemory[77];
val1 = val1 + sharedMemory[78];
val2 = val2 + sharedMemory[79];
val1 = val1 + sharedMemory[80];
val2 = val2 + sharedMemory[81];
val1 = val1 + sharedMemory[82];
val2 = val2 + sharedMemory[83];
val1 = val1 + sharedMemory[84];
val2 = val2 + sharedMemory[85];
val1 = val1 + sharedMemory[86];
val2 = val2 + sharedMemory[87];
val1 = val1 + sharedMemory[88];
val2 = val2 + sharedMemory[89];
val1 = val1 + sharedMemory[90];
val2 = val2 + sharedMemory[91];
val1 = val1 + sharedMemory[92];
val2 = val2 + sharedMemory[93];
val1 = val1 + sharedMemory[94];
val2 = val2 + sharedMemory[95];
val1 = val1 + sharedMemory[96];
val2 = val2 + sharedMemory[97];
val1 = val1 + sharedMemory[98];
val2 = val2 + sharedMemory[99];
val1 = val1 + sharedMemory[100];
val2 = val2 + sharedMemory[101];
val1 = val1 + sharedMemory[102];
val2 = val2 + sharedMemory[103];
val1 = val1 + sharedMemory[104];
val2 = val2 + sharedMemory[105];
val1 = val1 + sharedMemory[106];
val2 = val2 + sharedMemory[107];
val1 = val1 + sharedMemory[108];
val2 = val2 + sharedMemory[109];
val1 = val1 + sharedMemory[110];
val2 = val2 + sharedMemory[111];
val1 = val1 + sharedMemory[112];
val2 = val2 + sharedMemory[113];
val1 = val1 + sharedMemory[114];
val2 = val2 + sharedMemory[115];
val1 = val1 + sharedMemory[116];
val2 = val2 + sharedMemory[117];
val1 = val1 + sharedMemory[118];
val2 = val2 + sharedMemory[119];
val1 = val1 + sharedMemory[120];
val2 = val2 + sharedMemory[121];
val1 = val1 + sharedMemory[122];
val2 = val2 + sharedMemory[123];
val1 = val1 + sharedMemory[124];
val2 = val2 + sharedMemory[125];
val1 = val1 + sharedMemory[126];
val2 = val2 + sharedMemory[127];
val1 = val1 + sharedMemory[128];
val2 = val2 + sharedMemory[129];
val1 = val1 + sharedMemory[130];
val2 = val2 + sharedMemory[131];
val1 = val1 + sharedMemory[132];
val2 = val2 + sharedMemory[133];
val1 = val1 + sharedMemory[134];
val2 = val2 + sharedMemory[135];
val1 = val1 + sharedMemory[136];
val2 = val2 + sharedMemory[137];
val1 = val1 + sharedMemory[138];
val2 = val2 + sharedMemory[139];
val1 = val1 + sharedMemory[140];
val2 = val2 + sharedMemory[141];
val1 = val1 + sharedMemory[142];
val2 = val2 + sharedMemory[143];
val1 = val1 + sharedMemory[144];
val2 = val2 + sharedMemory[145];
val1 = val1 + sharedMemory[146];
val2 = val2 + sharedMemory[147];
val1 = val1 + sharedMemory[148];
val2 = val2 + sharedMemory[149];
val1 = val1 + sharedMemory[150];
val2 = val2 + sharedMemory[151];
val1 = val1 + sharedMemory[152];
val2 = val2 + sharedMemory[153];
val1 = val1 + sharedMemory[154];
val2 = val2 + sharedMemory[155];
val1 = val1 + sharedMemory[156];
val2 = val2 + sharedMemory[157];
val1 = val1 + sharedMemory[158];
val2 = val2 + sharedMemory[159];
val1 = val1 + sharedMemory[160];
val2 = val2 + sharedMemory[161];
val1 = val1 + sharedMemory[162];
val2 = val2 + sharedMemory[163];
val1 = val1 + sharedMemory[164];
val2 = val2 + sharedMemory[165];
val1 = val1 + sharedMemory[166];
val2 = val2 + sharedMemory[167];
val1 = val1 + sharedMemory[168];
val2 = val2 + sharedMemory[169];
val1 = val1 + sharedMemory[170];
val2 = val2 + sharedMemory[171];
val1 = val1 + sharedMemory[172];
val2 = val2 + sharedMemory[173];
val1 = val1 + sharedMemory[174];
val2 = val2 + sharedMemory[175];
val1 = val1 + sharedMemory[176];
val2 = val2 + sharedMemory[177];
val1 = val1 + sharedMemory[178];
val2 = val2 + sharedMemory[179];
val1 = val1 + sharedMemory[180];
val2 = val2 + sharedMemory[181];
val1 = val1 + sharedMemory[182];
val2 = val2 + sharedMemory[183];
val1 = val1 + sharedMemory[184];
val2 = val2 + sharedMemory[185];
val1 = val1 + sharedMemory[186];
val2 = val2 + sharedMemory[187];
val1 = val1 + sharedMemory[188];
val2 = val2 + sharedMemory[189];
val1 = val1 + sharedMemory[190];
val2 = val2 + sharedMemory[191];
val1 = val1 + sharedMemory[192];
val2 = val2 + sharedMemory[193];
val1 = val1 + sharedMemory[194];
val2 = val2 + sharedMemory[195];
val1 = val1 + sharedMemory[196];
val2 = val2 + sharedMemory[197];
val1 = val1 + sharedMemory[198];
val2 = val2 + sharedMemory[199];
val1 = val1 + sharedMemory[200];
val2 = val2 + sharedMemory[201];
val1 = val1 + sharedMemory[202];
val2 = val2 + sharedMemory[203];
val1 = val1 + sharedMemory[204];
val2 = val2 + sharedMemory[205];
val1 = val1 + sharedMemory[206];
val2 = val2 + sharedMemory[207];
val1 = val1 + sharedMemory[208];
val2 = val2 + sharedMemory[209];
val1 = val1 + sharedMemory[210];
val2 = val2 + sharedMemory[211];
val1 = val1 + sharedMemory[212];
val2 = val2 + sharedMemory[213];
val1 = val1 + sharedMemory[214];
val2 = val2 + sharedMemory[215];
val1 = val1 + sharedMemory[216];
val2 = val2 + sharedMemory[217];
val1 = val1 + sharedMemory[218];
val2 = val2 + sharedMemory[219];
val1 = val1 + sharedMemory[220];
val2 = val2 + sharedMemory[221];
val1 = val1 + sharedMemory[222];
val2 = val2 + sharedMemory[223];
val1 = val1 + sharedMemory[224];
val2 = val2 + sharedMemory[225];
val1 = val1 + sharedMemory[226];
val2 = val2 + sharedMemory[227];
val1 = val1 + sharedMemory[228];
val2 = val2 + sharedMemory[229];
val1 = val1 + sharedMemory[230];
val2 = val2 + sharedMemory[231];
val1 = val1 + sharedMemory[232];
val2 = val2 + sharedMemory[233];
val1 = val1 + sharedMemory[234];
val2 = val2 + sharedMemory[235];
val1 = val1 + sharedMemory[236];
val2 = val2 + sharedMemory[237];
val1 = val1 + sharedMemory[238];
val2 = val2 + sharedMemory[239];
val1 = val1 + sharedMemory[240];
val2 = val2 + sharedMemory[241];
val1 = val1 + sharedMemory[242];
val2 = val2 + sharedMemory[243];
val1 = val1 + sharedMemory[244];
val2 = val2 + sharedMemory[245];
val1 = val1 + sharedMemory[246];
val2 = val2 + sharedMemory[247];
val1 = val1 + sharedMemory[248];
val2 = val2 + sharedMemory[249];
val1 = val1 + sharedMemory[250];
val2 = val2 + sharedMemory[251];
val1 = val1 + sharedMemory[252];
val2 = val2 + sharedMemory[253];
val1 = val1 + sharedMemory[254];
val2 = val2 + sharedMemory[255];
output[gid] = val1 + val2;
}

__kernel void LDSBandwidth_linear(__local DATATYPE *sharedMemory,__global DATATYPE *output)
{
DATATYPE val1 = (DATATYPE)(0);
DATATYPE val2 = val1;
uint gid = get_global_id(0);
uint lid = get_local_id(0);
val1 = val1 + sharedMemory[0 + lid];
val2 = val2 + sharedMemory[1 + lid];
val1 = val1 + sharedMemory[2 + lid];
val2 = val2 + sharedMemory[3 + lid];
val1 = val1 + sharedMemory[4 + lid];
val2 = val2 + sharedMemory[5 + lid];
val1 = val1 + sharedMemory[6 + lid];
val2 = val2 + sharedMemory[7 + lid];
val1 = val1 + sharedMemory[8 + lid];
val2 = val2 + sharedMemory[9 + lid];
val1 = val1 + sharedMemory[10 + lid];
val2 = val2 + sharedMemory[11 + lid];
val1 = val1 + sharedMemory[12 + lid];
val2 = val2 + sharedMemory[13 + lid];
val1 = val1 + sharedMemory[14 + lid];
val2 = val2 + sharedMemory[15 + lid];
val1 = val1 + sharedMemory[16 + lid];
val2 = val2 + sharedMemory[17 + lid];
val1 = val1 + sharedMemory[18 + lid];
val2 = val2 + sharedMemory[19 + lid];
val1 = val1 + sharedMemory[20 + lid];
val2 = val2 + sharedMemory[21 + lid];
val1 = val1 + sharedMemory[22 + lid];
val2 = val2 + sharedMemory[23 + lid];
val1 = val1 + sharedMemory[24 + lid];
val2 = val2 + sharedMemory[25 + lid];
val1 = val1 + sharedMemory[26 + lid];
val2 = val2 + sharedMemory[27 + lid];
val1 = val1 + sharedMemory[28 + lid];
val2 = val2 + sharedMemory[29 + lid];
val1 = val1 + sharedMemory[30 + lid];
val2 = val2 + sharedMemory[31 + lid];
val1 = val1 + sharedMemory[32 + lid];
val2 = val2 + sharedMemory[33 + lid];
val1 = val1 + sharedMemory[34 + lid];
val2 = val2 + sharedMemory[35 + lid];
val1 = val1 + sharedMemory[36 + lid];
val2 = val2 + sharedMemory[37 + lid];
val1 = val1 + sharedMemory[38 + lid];
val2 = val2 + sharedMemory[39 + lid];
val1 = val1 + sharedMemory[40 + lid];
val2 = val2 + sharedMemory[41 + lid];
val1 = val1 + sharedMemory[42 + lid];
val2 = val2 + sharedMemory[43 + lid];
val1 = val1 + sharedMemory[44 + lid];
val2 = val2 + sharedMemory[45 + lid];
val1 = val1 + sharedMemory[46 + lid];
val2 = val2 + sharedMemory[47 + lid];
val1 = val1 + sharedMemory[48 + lid];
val2 = val2 + sharedMemory[49 + lid];
val1 = val1 + sharedMemory[50 + lid];
val2 = val2 + sharedMemory[51 + lid];
val1 = val1 + sharedMemory[52 + lid];
val2 = val2 + sharedMemory[53 + lid];
val1 = val1 + sharedMemory[54 + lid];
val2 = val2 + sharedMemory[55 + lid];
val1 = val1 + sharedMemory[56 + lid];
val2 = val2 + sharedMemory[57 + lid];
val1 = val1 + sharedMemory[58 + lid];
val2 = val2 + sharedMemory[59 + lid];
val1 = val1 + sharedMemory[60 + lid];
val2 = val2 + sharedMemory[61 + lid];
val1 = val1 + sharedMemory[62 + lid];
val2 = val2 + sharedMemory[63 + lid];
val1 = val1 + sharedMemory[64 + lid];
val2 = val2 + sharedMemory[65 + lid];
val1 = val1 + sharedMemory[66 + lid];
val2 = val2 + sharedMemory[67 + lid];
val1 = val1 + sharedMemory[68 + lid];
val2 = val2 + sharedMemory[69 + lid];
val1 = val1 + sharedMemory[70 + lid];
val2 = val2 + sharedMemory[71 + lid];
val1 = val1 + sharedMemory[72 + lid];
val2 = val2 + sharedMemory[73 + lid];
val1 = val1 + sharedMemory[74 + lid];
val2 = val2 + sharedMemory[75 + lid];
val1 = val1 + sharedMemory[76 + lid];
val2 = val2 + sharedMemory[77 + lid];
val1 = val1 + sharedMemory[78 + lid];
val2 = val2 + sharedMemory[79 + lid];
val1 = val1 + sharedMemory[80 + lid];
val2 = val2 + sharedMemory[81 + lid];
val1 = val1 + sharedMemory[82 + lid];
val2 = val2 + sharedMemory[83 + lid];
val1 = val1 + sharedMemory[84 + lid];
val2 = val2 + sharedMemory[85 + lid];
val1 = val1 + sharedMemory[86 + lid];
val2 = val2 + sharedMemory[87 + lid];
val1 = val1 + sharedMemory[88 + lid];
val2 = val2 + sharedMemory[89 + lid];
val1 = val1 + sharedMemory[90 + lid];
val2 = val2 + sharedMemory[91 + lid];
val1 = val1 + sharedMemory[92 + lid];
val2 = val2 + sharedMemory[93 + lid];
val1 = val1 + sharedMemory[94 + lid];
val2 = val2 + sharedMemory[95 + lid];
val1 = val1 + sharedMemory[96 + lid];
val2 = val2 + sharedMemory[97 + lid];
val1 = val1 + sharedMemory[98 + lid];
val2 = val2 + sharedMemory[99 + lid];
val1 = val1 + sharedMemory[100 + lid];
val2 = val2 + sharedMemory[101 + lid];
val1 = val1 + sharedMemory[102 + lid];
val2 = val2 + sharedMemory[103 + lid];
val1 = val1 + sharedMemory[104 + lid];
val2 = val2 + sharedMemory[105 + lid];
val1 = val1 + sharedMemory[106 + lid];
val2 = val2 + sharedMemory[107 + lid];
val1 = val1 + sharedMemory[108 + lid];
val2 = val2 + sharedMemory[109 + lid];
val1 = val1 + sharedMemory[110 + lid];
val2 = val2 + sharedMemory[111 + lid];
val1 = val1 + sharedMemory[112 + lid];
val2 = val2 + sharedMemory[113 + lid];
val1 = val1 + sharedMemory[114 + lid];
val2 = val2 + sharedMemory[115 + lid];
val1 = val1 + sharedMemory[116 + lid];
val2 = val2 + sharedMemory[117 + lid];
val1 = val1 + sharedMemory[118 + lid];
val2 = val2 + sharedMemory[119 + lid];
val1 = val1 + sharedMemory[120 + lid];
val2 = val2 + sharedMemory[121 + lid];
val1 = val1 + sharedMemory[122 + lid];
val2 = val2 + sharedMemory[123 + lid];
val1 = val1 + sharedMemory[124 + lid];
val2 = val2 + sharedMemory[125 + lid];
val1 = val1 + sharedMemory[126 + lid];
val2 = val2 + sharedMemory[127 + lid];
val1 = val1 + sharedMemory[128 + lid];
val2 = val2 + sharedMemory[129 + lid];
val1 = val1 + sharedMemory[130 + lid];
val2 = val2 + sharedMemory[131 + lid];
val1 = val1 + sharedMemory[132 + lid];
val2 = val2 + sharedMemory[133 + lid];
val1 = val1 + sharedMemory[134 + lid];
val2 = val2 + sharedMemory[135 + lid];
val1 = val1 + sharedMemory[136 + lid];
val2 = val2 + sharedMemory[137 + lid];
val1 = val1 + sharedMemory[138 + lid];
val2 = val2 + sharedMemory[139 + lid];
val1 = val1 + sharedMemory[140 + lid];
val2 = val2 + sharedMemory[141 + lid];
val1 = val1 + sharedMemory[142 + lid];
val2 = val2 + sharedMemory[143 + lid];
val1 = val1 + sharedMemory[144 + lid];
val2 = val2 + sharedMemory[145 + lid];
val1 = val1 + sharedMemory[146 + lid];
val2 = val2 + sharedMemory[147 + lid];
val1 = val1 + sharedMemory[148 + lid];
val2 = val2 + sharedMemory[149 + lid];
val1 = val1 + sharedMemory[150 + lid];
val2 = val2 + sharedMemory[151 + lid];
val1 = val1 + sharedMemory[152 + lid];
val2 = val2 + sharedMemory[153 + lid];
val1 = val1 + sharedMemory[154 + lid];
val2 = val2 + sharedMemory[155 + lid];
val1 = val1 + sharedMemory[156 + lid];
val2 = val2 + sharedMemory[157 + lid];
val1 = val1 + sharedMemory[158 + lid];
val2 = val2 + sharedMemory[159 + lid];
val1 = val1 + sharedMemory[160 + lid];
val2 = val2 + sharedMemory[161 + lid];
val1 = val1 + sharedMemory[162 + lid];
val2 = val2 + sharedMemory[163 + lid];
val1 = val1 + sharedMemory[164 + lid];
val2 = val2 + sharedMemory[165 + lid];
val1 = val1 + sharedMemory[166 + lid];
val2 = val2 + sharedMemory[167 + lid];
val1 = val1 + sharedMemory[168 + lid];
val2 = val2 + sharedMemory[169 + lid];
val1 = val1 + sharedMemory[170 + lid];
val2 = val2 + sharedMemory[171 + lid];
val1 = val1 + sharedMemory[172 + lid];
val2 = val2 + sharedMemory[173 + lid];
val1 = val1 + sharedMemory[174 + lid];
val2 = val2 + sharedMemory[175 + lid];
val1 = val1 + sharedMemory[176 + lid];
val2 = val2 + sharedMemory[177 + lid];
val1 = val1 + sharedMemory[178 + lid];
val2 = val2 + sharedMemory[179 + lid];
val1 = val1 + sharedMemory[180 + lid];
val2 = val2 + sharedMemory[181 + lid];
val1 = val1 + sharedMemory[182 + lid];
val2 = val2 + sharedMemory[183 + lid];
val1 = val1 + sharedMemory[184 + lid];
val2 = val2 + sharedMemory[185 + lid];
val1 = val1 + sharedMemory[186 + lid];
val2 = val2 + sharedMemory[187 + lid];
val1 = val1 + sharedMemory[188 + lid];
val2 = val2 + sharedMemory[189 + lid];
val1 = val1 + sharedMemory[190 + lid];
val2 = val2 + sharedMemory[191 + lid];
val1 = val1 + sharedMemory[192 + lid];
val2 = val2 + sharedMemory[193 + lid];
val1 = val1 + sharedMemory[194 + lid];
val2 = val2 + sharedMemory[195 + lid];
val1 = val1 + sharedMemory[196 + lid];
val2 = val2 + sharedMemory[197 + lid];
val1 = val1 + sharedMemory[198 + lid];
val2 = val2 + sharedMemory[199 + lid];
val1 = val1 + sharedMemory[200 + lid];
val2 = val2 + sharedMemory[201 + lid];
val1 = val1 + sharedMemory[202 + lid];
val2 = val2 + sharedMemory[203 + lid];
val1 = val1 + sharedMemory[204 + lid];
val2 = val2 + sharedMemory[205 + lid];
val1 = val1 + sharedMemory[206 + lid];
val2 = val2 + sharedMemory[207 + lid];
val1 = val1 + sharedMemory[208 + lid];
val2 = val2 + sharedMemory[209 + lid];
val1 = val1 + sharedMemory[210 + lid];
val2 = val2 + sharedMemory[211 + lid];
val1 = val1 + sharedMemory[212 + lid];
val2 = val2 + sharedMemory[213 + lid];
val1 = val1 + sharedMemory[214 + lid];
val2 = val2 + sharedMemory[215 + lid];
val1 = val1 + sharedMemory[216 + lid];
val2 = val2 + sharedMemory[217 + lid];
val1 = val1 + sharedMemory[218 + lid];
val2 = val2 + sharedMemory[219 + lid];
val1 = val1 + sharedMemory[220 + lid];
val2 = val2 + sharedMemory[221 + lid];
val1 = val1 + sharedMemory[222 + lid];
val2 = val2 + sharedMemory[223 + lid];
val1 = val1 + sharedMemory[224 + lid];
val2 = val2 + sharedMemory[225 + lid];
val1 = val1 + sharedMemory[226 + lid];
val2 = val2 + sharedMemory[227 + lid];
val1 = val1 + sharedMemory[228 + lid];
val2 = val2 + sharedMemory[229 + lid];
val1 = val1 + sharedMemory[230 + lid];
val2 = val2 + sharedMemory[231 + lid];
val1 = val1 + sharedMemory[232 + lid];
val2 = val2 + sharedMemory[233 + lid];
val1 = val1 + sharedMemory[234 + lid];
val2 = val2 + sharedMemory[235 + lid];
val1 = val1 + sharedMemory[236 + lid];
val2 = val2 + sharedMemory[237 + lid];
val1 = val1 + sharedMemory[238 + lid];
val2 = val2 + sharedMemory[239 + lid];
val1 = val1 + sharedMemory[240 + lid];
val2 = val2 + sharedMemory[241 + lid];
val1 = val1 + sharedMemory[242 + lid];
val2 = val2 + sharedMemory[243 + lid];
val1 = val1 + sharedMemory[244 + lid];
val2 = val2 + sharedMemory[245 + lid];
val1 = val1 + sharedMemory[246 + lid];
val2 = val2 + sharedMemory[247 + lid];
val1 = val1 + sharedMemory[248 + lid];
val2 = val2 + sharedMemory[249 + lid];
val1 = val1 + sharedMemory[250 + lid];
val2 = val2 + sharedMemory[251 + lid];
val1 = val1 + sharedMemory[252 + lid];
val2 = val2 + sharedMemory[253 + lid];
val1 = val1 + sharedMemory[254 + lid];
val2 = val2 + sharedMemory[255 + lid];
output[gid] = val1 + val2;
}

__kernel void LDSBandwidth_single_verify(__local DATATYPE *sharedMemory, __global DATATYPE *output)
{
DATATYPE val1 = (DATATYPE)(0);
DATATYPE val2 = val1;
uint gid = get_global_id(0);
uint lid = get_local_id(0);
for(int i = 0; i < 256; i++)
    sharedMemory[i] = 0;
barrier(CLK_LOCAL_MEM_FENCE);
val1 = val1 + sharedMemory[0];
val2 = val2 + sharedMemory[1];
val1 = val1 + sharedMemory[2];
val2 = val2 + sharedMemory[3];
val1 = val1 + sharedMemory[4];
val2 = val2 + sharedMemory[5];
val1 = val1 + sharedMemory[6];
val2 = val2 + sharedMemory[7];
val1 = val1 + sharedMemory[8];
val2 = val2 + sharedMemory[9];
val1 = val1 + sharedMemory[10];
val2 = val2 + sharedMemory[11];
val1 = val1 + sharedMemory[12];
val2 = val2 + sharedMemory[13];
val1 = val1 + sharedMemory[14];
val2 = val2 + sharedMemory[15];
val1 = val1 + sharedMemory[16];
val2 = val2 + sharedMemory[17];
val1 = val1 + sharedMemory[18];
val2 = val2 + sharedMemory[19];
val1 = val1 + sharedMemory[20];
val2 = val2 + sharedMemory[21];
val1 = val1 + sharedMemory[22];
val2 = val2 + sharedMemory[23];
val1 = val1 + sharedMemory[24];
val2 = val2 + sharedMemory[25];
val1 = val1 + sharedMemory[26];
val2 = val2 + sharedMemory[27];
val1 = val1 + sharedMemory[28];
val2 = val2 + sharedMemory[29];
val1 = val1 + sharedMemory[30];
val2 = val2 + sharedMemory[31];
val1 = val1 + sharedMemory[32];
val2 = val2 + sharedMemory[33];
val1 = val1 + sharedMemory[34];
val2 = val2 + sharedMemory[35];
val1 = val1 + sharedMemory[36];
val2 = val2 + sharedMemory[37];
val1 = val1 + sharedMemory[38];
val2 = val2 + sharedMemory[39];
val1 = val1 + sharedMemory[40];
val2 = val2 + sharedMemory[41];
val1 = val1 + sharedMemory[42];
val2 = val2 + sharedMemory[43];
val1 = val1 + sharedMemory[44];
val2 = val2 + sharedMemory[45];
val1 = val1 + sharedMemory[46];
val2 = val2 + sharedMemory[47];
val1 = val1 + sharedMemory[48];
val2 = val2 + sharedMemory[49];
val1 = val1 + sharedMemory[50];
val2 = val2 + sharedMemory[51];
val1 = val1 + sharedMemory[52];
val2 = val2 + sharedMemory[53];
val1 = val1 + sharedMemory[54];
val2 = val2 + sharedMemory[55];
val1 = val1 + sharedMemory[56];
val2 = val2 + sharedMemory[57];
val1 = val1 + sharedMemory[58];
val2 = val2 + sharedMemory[59];
val1 = val1 + sharedMemory[60];
val2 = val2 + sharedMemory[61];
val1 = val1 + sharedMemory[62];
val2 = val2 + sharedMemory[63];
val1 = val1 + sharedMemory[64];
val2 = val2 + sharedMemory[65];
val1 = val1 + sharedMemory[66];
val2 = val2 + sharedMemory[67];
val1 = val1 + sharedMemory[68];
val2 = val2 + sharedMemory[69];
val1 = val1 + sharedMemory[70];
val2 = val2 + sharedMemory[71];
val1 = val1 + sharedMemory[72];
val2 = val2 + sharedMemory[73];
val1 = val1 + sharedMemory[74];
val2 = val2 + sharedMemory[75];
val1 = val1 + sharedMemory[76];
val2 = val2 + sharedMemory[77];
val1 = val1 + sharedMemory[78];
val2 = val2 + sharedMemory[79];
val1 = val1 + sharedMemory[80];
val2 = val2 + sharedMemory[81];
val1 = val1 + sharedMemory[82];
val2 = val2 + sharedMemory[83];
val1 = val1 + sharedMemory[84];
val2 = val2 + sharedMemory[85];
val1 = val1 + sharedMemory[86];
val2 = val2 + sharedMemory[87];
val1 = val1 + sharedMemory[88];
val2 = val2 + sharedMemory[89];
val1 = val1 + sharedMemory[90];
val2 = val2 + sharedMemory[91];
val1 = val1 + sharedMemory[92];
val2 = val2 + sharedMemory[93];
val1 = val1 + sharedMemory[94];
val2 = val2 + sharedMemory[95];
val1 = val1 + sharedMemory[96];
val2 = val2 + sharedMemory[97];
val1 = val1 + sharedMemory[98];
val2 = val2 + sharedMemory[99];
val1 = val1 + sharedMemory[100];
val2 = val2 + sharedMemory[101];
val1 = val1 + sharedMemory[102];
val2 = val2 + sharedMemory[103];
val1 = val1 + sharedMemory[104];
val2 = val2 + sharedMemory[105];
val1 = val1 + sharedMemory[106];
val2 = val2 + sharedMemory[107];
val1 = val1 + sharedMemory[108];
val2 = val2 + sharedMemory[109];
val1 = val1 + sharedMemory[110];
val2 = val2 + sharedMemory[111];
val1 = val1 + sharedMemory[112];
val2 = val2 + sharedMemory[113];
val1 = val1 + sharedMemory[114];
val2 = val2 + sharedMemory[115];
val1 = val1 + sharedMemory[116];
val2 = val2 + sharedMemory[117];
val1 = val1 + sharedMemory[118];
val2 = val2 + sharedMemory[119];
val1 = val1 + sharedMemory[120];
val2 = val2 + sharedMemory[121];
val1 = val1 + sharedMemory[122];
val2 = val2 + sharedMemory[123];
val1 = val1 + sharedMemory[124];
val2 = val2 + sharedMemory[125];
val1 = val1 + sharedMemory[126];
val2 = val2 + sharedMemory[127];
val1 = val1 + sharedMemory[128];
val2 = val2 + sharedMemory[129];
val1 = val1 + sharedMemory[130];
val2 = val2 + sharedMemory[131];
val1 = val1 + sharedMemory[132];
val2 = val2 + sharedMemory[133];
val1 = val1 + sharedMemory[134];
val2 = val2 + sharedMemory[135];
val1 = val1 + sharedMemory[136];
val2 = val2 + sharedMemory[137];
val1 = val1 + sharedMemory[138];
val2 = val2 + sharedMemory[139];
val1 = val1 + sharedMemory[140];
val2 = val2 + sharedMemory[141];
val1 = val1 + sharedMemory[142];
val2 = val2 + sharedMemory[143];
val1 = val1 + sharedMemory[144];
val2 = val2 + sharedMemory[145];
val1 = val1 + sharedMemory[146];
val2 = val2 + sharedMemory[147];
val1 = val1 + sharedMemory[148];
val2 = val2 + sharedMemory[149];
val1 = val1 + sharedMemory[150];
val2 = val2 + sharedMemory[151];
val1 = val1 + sharedMemory[152];
val2 = val2 + sharedMemory[153];
val1 = val1 + sharedMemory[154];
val2 = val2 + sharedMemory[155];
val1 = val1 + sharedMemory[156];
val2 = val2 + sharedMemory[157];
val1 = val1 + sharedMemory[158];
val2 = val2 + sharedMemory[159];
val1 = val1 + sharedMemory[160];
val2 = val2 + sharedMemory[161];
val1 = val1 + sharedMemory[162];
val2 = val2 + sharedMemory[163];
val1 = val1 + sharedMemory[164];
val2 = val2 + sharedMemory[165];
val1 = val1 + sharedMemory[166];
val2 = val2 + sharedMemory[167];
val1 = val1 + sharedMemory[168];
val2 = val2 + sharedMemory[169];
val1 = val1 + sharedMemory[170];
val2 = val2 + sharedMemory[171];
val1 = val1 + sharedMemory[172];
val2 = val2 + sharedMemory[173];
val1 = val1 + sharedMemory[174];
val2 = val2 + sharedMemory[175];
val1 = val1 + sharedMemory[176];
val2 = val2 + sharedMemory[177];
val1 = val1 + sharedMemory[178];
val2 = val2 + sharedMemory[179];
val1 = val1 + sharedMemory[180];
val2 = val2 + sharedMemory[181];
val1 = val1 + sharedMemory[182];
val2 = val2 + sharedMemory[183];
val1 = val1 + sharedMemory[184];
val2 = val2 + sharedMemory[185];
val1 = val1 + sharedMemory[186];
val2 = val2 + sharedMemory[187];
val1 = val1 + sharedMemory[188];
val2 = val2 + sharedMemory[189];
val1 = val1 + sharedMemory[190];
val2 = val2 + sharedMemory[191];
val1 = val1 + sharedMemory[192];
val2 = val2 + sharedMemory[193];
val1 = val1 + sharedMemory[194];
val2 = val2 + sharedMemory[195];
val1 = val1 + sharedMemory[196];
val2 = val2 + sharedMemory[197];
val1 = val1 + sharedMemory[198];
val2 = val2 + sharedMemory[199];
val1 = val1 + sharedMemory[200];
val2 = val2 + sharedMemory[201];
val1 = val1 + sharedMemory[202];
val2 = val2 + sharedMemory[203];
val1 = val1 + sharedMemory[204];
val2 = val2 + sharedMemory[205];
val1 = val1 + sharedMemory[206];
val2 = val2 + sharedMemory[207];
val1 = val1 + sharedMemory[208];
val2 = val2 + sharedMemory[209];
val1 = val1 + sharedMemory[210];
val2 = val2 + sharedMemory[211];
val1 = val1 + sharedMemory[212];
val2 = val2 + sharedMemory[213];
val1 = val1 + sharedMemory[214];
val2 = val2 + sharedMemory[215];
val1 = val1 + sharedMemory[216];
val2 = val2 + sharedMemory[217];
val1 = val1 + sharedMemory[218];
val2 = val2 + sharedMemory[219];
val1 = val1 + sharedMemory[220];
val2 = val2 + sharedMemory[221];
val1 = val1 + sharedMemory[222];
val2 = val2 + sharedMemory[223];
val1 = val1 + sharedMemory[224];
val2 = val2 + sharedMemory[225];
val1 = val1 + sharedMemory[226];
val2 = val2 + sharedMemory[227];
val1 = val1 + sharedMemory[228];
val2 = val2 + sharedMemory[229];
val1 = val1 + sharedMemory[230];
val2 = val2 + sharedMemory[231];
val1 = val1 + sharedMemory[232];
val2 = val2 + sharedMemory[233];
val1 = val1 + sharedMemory[234];
val2 = val2 + sharedMemory[235];
val1 = val1 + sharedMemory[236];
val2 = val2 + sharedMemory[237];
val1 = val1 + sharedMemory[238];
val2 = val2 + sharedMemory[239];
val1 = val1 + sharedMemory[240];
val2 = val2 + sharedMemory[241];
val1 = val1 + sharedMemory[242];
val2 = val2 + sharedMemory[243];
val1 = val1 + sharedMemory[244];
val2 = val2 + sharedMemory[245];
val1 = val1 + sharedMemory[246];
val2 = val2 + sharedMemory[247];
val1 = val1 + sharedMemory[248];
val2 = val2 + sharedMemory[249];
val1 = val1 + sharedMemory[250];
val2 = val2 + sharedMemory[251];
val1 = val1 + sharedMemory[252];
val2 = val2 + sharedMemory[253];
val1 = val1 + sharedMemory[254];
val2 = val2 + sharedMemory[255];
output[gid] = val1 + val2;
}

__kernel void LDSBandwidth_linear_verify(__local DATATYPE *sharedMemory,__global DATATYPE *output)
{
DATATYPE val1 = (DATATYPE)(0);
DATATYPE val2 = val1;
uint gid = get_global_id(0);
uint lid = get_local_id(0);
for(int i = 0; i < 256 + get_local_size(0); i++)
    sharedMemory[i] = 0;
barrier(CLK_LOCAL_MEM_FENCE);
val1 = val1 + sharedMemory[0 + lid];
val2 = val2 + sharedMemory[1 + lid];
val1 = val1 + sharedMemory[2 + lid];
val2 = val2 + sharedMemory[3 + lid];
val1 = val1 + sharedMemory[4 + lid];
val2 = val2 + sharedMemory[5 + lid];
val1 = val1 + sharedMemory[6 + lid];
val2 = val2 + sharedMemory[7 + lid];
val1 = val1 + sharedMemory[8 + lid];
val2 = val2 + sharedMemory[9 + lid];
val1 = val1 + sharedMemory[10 + lid];
val2 = val2 + sharedMemory[11 + lid];
val1 = val1 + sharedMemory[12 + lid];
val2 = val2 + sharedMemory[13 + lid];
val1 = val1 + sharedMemory[14 + lid];
val2 = val2 + sharedMemory[15 + lid];
val1 = val1 + sharedMemory[16 + lid];
val2 = val2 + sharedMemory[17 + lid];
val1 = val1 + sharedMemory[18 + lid];
val2 = val2 + sharedMemory[19 + lid];
val1 = val1 + sharedMemory[20 + lid];
val2 = val2 + sharedMemory[21 + lid];
val1 = val1 + sharedMemory[22 + lid];
val2 = val2 + sharedMemory[23 + lid];
val1 = val1 + sharedMemory[24 + lid];
val2 = val2 + sharedMemory[25 + lid];
val1 = val1 + sharedMemory[26 + lid];
val2 = val2 + sharedMemory[27 + lid];
val1 = val1 + sharedMemory[28 + lid];
val2 = val2 + sharedMemory[29 + lid];
val1 = val1 + sharedMemory[30 + lid];
val2 = val2 + sharedMemory[31 + lid];
val1 = val1 + sharedMemory[32 + lid];
val2 = val2 + sharedMemory[33 + lid];
val1 = val1 + sharedMemory[34 + lid];
val2 = val2 + sharedMemory[35 + lid];
val1 = val1 + sharedMemory[36 + lid];
val2 = val2 + sharedMemory[37 + lid];
val1 = val1 + sharedMemory[38 + lid];
val2 = val2 + sharedMemory[39 + lid];
val1 = val1 + sharedMemory[40 + lid];
val2 = val2 + sharedMemory[41 + lid];
val1 = val1 + sharedMemory[42 + lid];
val2 = val2 + sharedMemory[43 + lid];
val1 = val1 + sharedMemory[44 + lid];
val2 = val2 + sharedMemory[45 + lid];
val1 = val1 + sharedMemory[46 + lid];
val2 = val2 + sharedMemory[47 + lid];
val1 = val1 + sharedMemory[48 + lid];
val2 = val2 + sharedMemory[49 + lid];
val1 = val1 + sharedMemory[50 + lid];
val2 = val2 + sharedMemory[51 + lid];
val1 = val1 + sharedMemory[52 + lid];
val2 = val2 + sharedMemory[53 + lid];
val1 = val1 + sharedMemory[54 + lid];
val2 = val2 + sharedMemory[55 + lid];
val1 = val1 + sharedMemory[56 + lid];
val2 = val2 + sharedMemory[57 + lid];
val1 = val1 + sharedMemory[58 + lid];
val2 = val2 + sharedMemory[59 + lid];
val1 = val1 + sharedMemory[60 + lid];
val2 = val2 + sharedMemory[61 + lid];
val1 = val1 + sharedMemory[62 + lid];
val2 = val2 + sharedMemory[63 + lid];
val1 = val1 + sharedMemory[64 + lid];
val2 = val2 + sharedMemory[65 + lid];
val1 = val1 + sharedMemory[66 + lid];
val2 = val2 + sharedMemory[67 + lid];
val1 = val1 + sharedMemory[68 + lid];
val2 = val2 + sharedMemory[69 + lid];
val1 = val1 + sharedMemory[70 + lid];
val2 = val2 + sharedMemory[71 + lid];
val1 = val1 + sharedMemory[72 + lid];
val2 = val2 + sharedMemory[73 + lid];
val1 = val1 + sharedMemory[74 + lid];
val2 = val2 + sharedMemory[75 + lid];
val1 = val1 + sharedMemory[76 + lid];
val2 = val2 + sharedMemory[77 + lid];
val1 = val1 + sharedMemory[78 + lid];
val2 = val2 + sharedMemory[79 + lid];
val1 = val1 + sharedMemory[80 + lid];
val2 = val2 + sharedMemory[81 + lid];
val1 = val1 + sharedMemory[82 + lid];
val2 = val2 + sharedMemory[83 + lid];
val1 = val1 + sharedMemory[84 + lid];
val2 = val2 + sharedMemory[85 + lid];
val1 = val1 + sharedMemory[86 + lid];
val2 = val2 + sharedMemory[87 + lid];
val1 = val1 + sharedMemory[88 + lid];
val2 = val2 + sharedMemory[89 + lid];
val1 = val1 + sharedMemory[90 + lid];
val2 = val2 + sharedMemory[91 + lid];
val1 = val1 + sharedMemory[92 + lid];
val2 = val2 + sharedMemory[93 + lid];
val1 = val1 + sharedMemory[94 + lid];
val2 = val2 + sharedMemory[95 + lid];
val1 = val1 + sharedMemory[96 + lid];
val2 = val2 + sharedMemory[97 + lid];
val1 = val1 + sharedMemory[98 + lid];
val2 = val2 + sharedMemory[99 + lid];
val1 = val1 + sharedMemory[100 + lid];
val2 = val2 + sharedMemory[101 + lid];
val1 = val1 + sharedMemory[102 + lid];
val2 = val2 + sharedMemory[103 + lid];
val1 = val1 + sharedMemory[104 + lid];
val2 = val2 + sharedMemory[105 + lid];
val1 = val1 + sharedMemory[106 + lid];
val2 = val2 + sharedMemory[107 + lid];
val1 = val1 + sharedMemory[108 + lid];
val2 = val2 + sharedMemory[109 + lid];
val1 = val1 + sharedMemory[110 + lid];
val2 = val2 + sharedMemory[111 + lid];
val1 = val1 + sharedMemory[112 + lid];
val2 = val2 + sharedMemory[113 + lid];
val1 = val1 + sharedMemory[114 + lid];
val2 = val2 + sharedMemory[115 + lid];
val1 = val1 + sharedMemory[116 + lid];
val2 = val2 + sharedMemory[117 + lid];
val1 = val1 + sharedMemory[118 + lid];
val2 = val2 + sharedMemory[119 + lid];
val1 = val1 + sharedMemory[120 + lid];
val2 = val2 + sharedMemory[121 + lid];
val1 = val1 + sharedMemory[122 + lid];
val2 = val2 + sharedMemory[123 + lid];
val1 = val1 + sharedMemory[124 + lid];
val2 = val2 + sharedMemory[125 + lid];
val1 = val1 + sharedMemory[126 + lid];
val2 = val2 + sharedMemory[127 + lid];
val1 = val1 + sharedMemory[128 + lid];
val2 = val2 + sharedMemory[129 + lid];
val1 = val1 + sharedMemory[130 + lid];
val2 = val2 + sharedMemory[131 + lid];
val1 = val1 + sharedMemory[132 + lid];
val2 = val2 + sharedMemory[133 + lid];
val1 = val1 + sharedMemory[134 + lid];
val2 = val2 + sharedMemory[135 + lid];
val1 = val1 + sharedMemory[136 + lid];
val2 = val2 + sharedMemory[137 + lid];
val1 = val1 + sharedMemory[138 + lid];
val2 = val2 + sharedMemory[139 + lid];
val1 = val1 + sharedMemory[140 + lid];
val2 = val2 + sharedMemory[141 + lid];
val1 = val1 + sharedMemory[142 + lid];
val2 = val2 + sharedMemory[143 + lid];
val1 = val1 + sharedMemory[144 + lid];
val2 = val2 + sharedMemory[145 + lid];
val1 = val1 + sharedMemory[146 + lid];
val2 = val2 + sharedMemory[147 + lid];
val1 = val1 + sharedMemory[148 + lid];
val2 = val2 + sharedMemory[149 + lid];
val1 = val1 + sharedMemory[150 + lid];
val2 = val2 + sharedMemory[151 + lid];
val1 = val1 + sharedMemory[152 + lid];
val2 = val2 + sharedMemory[153 + lid];
val1 = val1 + sharedMemory[154 + lid];
val2 = val2 + sharedMemory[155 + lid];
val1 = val1 + sharedMemory[156 + lid];
val2 = val2 + sharedMemory[157 + lid];
val1 = val1 + sharedMemory[158 + lid];
val2 = val2 + sharedMemory[159 + lid];
val1 = val1 + sharedMemory[160 + lid];
val2 = val2 + sharedMemory[161 + lid];
val1 = val1 + sharedMemory[162 + lid];
val2 = val2 + sharedMemory[163 + lid];
val1 = val1 + sharedMemory[164 + lid];
val2 = val2 + sharedMemory[165 + lid];
val1 = val1 + sharedMemory[166 + lid];
val2 = val2 + sharedMemory[167 + lid];
val1 = val1 + sharedMemory[168 + lid];
val2 = val2 + sharedMemory[169 + lid];
val1 = val1 + sharedMemory[170 + lid];
val2 = val2 + sharedMemory[171 + lid];
val1 = val1 + sharedMemory[172 + lid];
val2 = val2 + sharedMemory[173 + lid];
val1 = val1 + sharedMemory[174 + lid];
val2 = val2 + sharedMemory[175 + lid];
val1 = val1 + sharedMemory[176 + lid];
val2 = val2 + sharedMemory[177 + lid];
val1 = val1 + sharedMemory[178 + lid];
val2 = val2 + sharedMemory[179 + lid];
val1 = val1 + sharedMemory[180 + lid];
val2 = val2 + sharedMemory[181 + lid];
val1 = val1 + sharedMemory[182 + lid];
val2 = val2 + sharedMemory[183 + lid];
val1 = val1 + sharedMemory[184 + lid];
val2 = val2 + sharedMemory[185 + lid];
val1 = val1 + sharedMemory[186 + lid];
val2 = val2 + sharedMemory[187 + lid];
val1 = val1 + sharedMemory[188 + lid];
val2 = val2 + sharedMemory[189 + lid];
val1 = val1 + sharedMemory[190 + lid];
val2 = val2 + sharedMemory[191 + lid];
val1 = val1 + sharedMemory[192 + lid];
val2 = val2 + sharedMemory[193 + lid];
val1 = val1 + sharedMemory[194 + lid];
val2 = val2 + sharedMemory[195 + lid];
val1 = val1 + sharedMemory[196 + lid];
val2 = val2 + sharedMemory[197 + lid];
val1 = val1 + sharedMemory[198 + lid];
val2 = val2 + sharedMemory[199 + lid];
val1 = val1 + sharedMemory[200 + lid];
val2 = val2 + sharedMemory[201 + lid];
val1 = val1 + sharedMemory[202 + lid];
val2 = val2 + sharedMemory[203 + lid];
val1 = val1 + sharedMemory[204 + lid];
val2 = val2 + sharedMemory[205 + lid];
val1 = val1 + sharedMemory[206 + lid];
val2 = val2 + sharedMemory[207 + lid];
val1 = val1 + sharedMemory[208 + lid];
val2 = val2 + sharedMemory[209 + lid];
val1 = val1 + sharedMemory[210 + lid];
val2 = val2 + sharedMemory[211 + lid];
val1 = val1 + sharedMemory[212 + lid];
val2 = val2 + sharedMemory[213 + lid];
val1 = val1 + sharedMemory[214 + lid];
val2 = val2 + sharedMemory[215 + lid];
val1 = val1 + sharedMemory[216 + lid];
val2 = val2 + sharedMemory[217 + lid];
val1 = val1 + sharedMemory[218 + lid];
val2 = val2 + sharedMemory[219 + lid];
val1 = val1 + sharedMemory[220 + lid];
val2 = val2 + sharedMemory[221 + lid];
val1 = val1 + sharedMemory[222 + lid];
val2 = val2 + sharedMemory[223 + lid];
val1 = val1 + sharedMemory[224 + lid];
val2 = val2 + sharedMemory[225 + lid];
val1 = val1 + sharedMemory[226 + lid];
val2 = val2 + sharedMemory[227 + lid];
val1 = val1 + sharedMemory[228 + lid];
val2 = val2 + sharedMemory[229 + lid];
val1 = val1 + sharedMemory[230 + lid];
val2 = val2 + sharedMemory[231 + lid];
val1 = val1 + sharedMemory[232 + lid];
val2 = val2 + sharedMemory[233 + lid];
val1 = val1 + sharedMemory[234 + lid];
val2 = val2 + sharedMemory[235 + lid];
val1 = val1 + sharedMemory[236 + lid];
val2 = val2 + sharedMemory[237 + lid];
val1 = val1 + sharedMemory[238 + lid];
val2 = val2 + sharedMemory[239 + lid];
val1 = val1 + sharedMemory[240 + lid];
val2 = val2 + sharedMemory[241 + lid];
val1 = val1 + sharedMemory[242 + lid];
val2 = val2 + sharedMemory[243 + lid];
val1 = val1 + sharedMemory[244 + lid];
val2 = val2 + sharedMemory[245 + lid];
val1 = val1 + sharedMemory[246 + lid];
val2 = val2 + sharedMemory[247 + lid];
val1 = val1 + sharedMemory[248 + lid];
val2 = val2 + sharedMemory[249 + lid];
val1 = val1 + sharedMemory[250 + lid];
val2 = val2 + sharedMemory[251 + lid];
val1 = val1 + sharedMemory[252 + lid];
val2 = val2 + sharedMemory[253 + lid];
val1 = val1 + sharedMemory[254 + lid];
val2 = val2 + sharedMemory[255 + lid];
output[gid] = val1 + val2;
}