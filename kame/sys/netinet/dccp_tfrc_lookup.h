/*	$KAME: dccp_tfrc_lookup.h,v 1.3 2003/10/18 08:16:17 itojun Exp $	*/

/*
 * Copyright (c) 2003  Nils-Erik Mattsson 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Id: dccp_tfrc_lookup.h,v 1.5 2003/05/28 17:37:43 nilmat-8 Exp
 */

#ifndef _NETINET_DCCP_TFRC_LOOKUP_H_
#define _NETINET_DCCP_TFRC_LOOKUP_H_

const double fsmall_table[] ={
  0,
  0.00163305194960931,
  0.00230956735363637,
  0.00282873259487707,
  0.00326645662574537,
  0.00365214098377853,
  0.00400086400001593,
  0.00432158256340322,
  0.00462013236858081,
  0.00490056675498551,
  0.00516583682704458,
  0.00541817034936234,
  0.00565929801070833,
  0.00589059608718062,
  0.00611318041778383,
  0.00632797058060318,
  0.00653573506415987,
  0.00673712389086595,
  0.00693269270671348,
  0.0071229209166037,
  0.00730822557103113,
  0.00748897216079408,
  0.00766548312170872,
  0.00783804461648552,
  0.00800691200203843,
  0.00817231428085712,
  0.00833445775806404,
  0.00849352907081277,
  0.00864969771687212,
  0.00880311818001545,
  0.0089539317281132,
  0.00910226794349901,
  0.00924824603277504,
  0.00939197595370597,
  0.009533559389484,
  0.00967309059489451,
  0.00981065713438519,
  0.00994634052845154,
  0.0100802168218855,
  0.0102123570851299,
  0.0103428278581192,
  0.010471691544469,
  0.0105990067626397,
  0.0107248286596785,
  0.0108492091922994,
  0.0109721973793658,
  0.0110938395292506,
  0.0112141794450653,
  0.0113332586103332,
  0.0114511163573388,
  0.0115677900200876,
  0.0116833150735621,
  0.011797725260745,
  0.0119110527086984,
  0.0120233280348285,
  0.0121345804443331,
  0.0122448378197079,
  0.0123541268030903,
  0.0124624728721277,
  0.0125699004099823,
  0.0126764327700174,
  0.0127820923356504,
  0.0128869005758063,
  0.0129908780963614,
  0.0130940446879258,
  0.0131964193702772,
  0.01329802043373,
  0.0133988654776928,
  0.0134989714466453,
  0.0135983546637418,
  0.0136970308622305,
  0.0137950152148595,
  0.0138923223614248,
  0.0139889664346018,
  0.0140849610841894,
  0.0141803194998843,
  0.0142750544326924,
  0.0143691782150771,
  0.0144627027799322,
  0.0145556396784645,
  0.0146480000970597,
  0.014739794873203,
  0.0148310345105165,
  0.0149217291929752,
  0.0150118887983524,
  0.0151015229109484,
  0.0151906408336458,
  0.0152792515993363,
  0.0153673639817576,
  0.0154549865057777,
  0.0155421274571605,
  0.0156287948918437,
  0.0157149966447599,
  0.0158007403382266,
  0.0158860333899315,
  0.0159708830205358,
  0.0160552962609193,
  0.0161392799590851,
  0.0162228407867464,
  0.0163059852456097,
  0.0163887196733746,
  0.0164710502494631,
  0.0165529830004946,
  0.0166345238055199,
  0.0167156784010267,
  0.0167964523857295,
  0.0168768512251538,
  0.0169568802560266,
  0.0170365446904829,
  0.0171158496200964,
  0.0171948000197448,
  0.0172734007513173,
  0.0173516565672713,
  0.0174295721140473,
  0.017507151935348,
  0.0175844004752871,
  0.017661322081417,
  0.0177379210076372,
  0.0178142014169926,
  0.0178901673843644,
  0.0179658228990597,
  0.0180411718673042,
  0.0181162181146423,
  0.0181909653882486,
  0.0182654173591552,
  0.0183395776243984,
  0.018413449709088,
  0.0184870370684027,
  0.0185603430895154,
  0.0186333710934504,
  0.0187061243368762,
  0.0187786060138361,
  0.0188508192574198,
  0.0189227671413778,
  0.0189944526816812,
  0.0190658788380296,
  0.019137048515308,
  0.0192079645649969,
  0.0192786297865344,
  0.0193490469286358,
  0.0194192186905687,
  0.0194891477233888,
  0.0195588366311352,
  0.0196282879719884,
  0.0196975042593915,
  0.0197664879631372,
  0.01983524151042,
  0.0199037672868568,
  0.0199720676374761,
  0.0200401448676771,
  0.02010800124416,
  0.020175638995828,
  0.0202430603146631,
  0.0203102673565755,
  0.0203772622422282,
  0.0204440470578377,
  0.0205106238559511,
  0.0205769946562011,
  0.0206431614460395,
  0.0207091261814494,
  0.0207748907876375,
  0.020840457159707,
  0.020905827163312,
  0.0209710026352927,
  0.0210359853842945,
  0.0211007771913689,
  0.0211653798105588,
  0.0212297949694677,
  0.0212940243698132,
  0.0213580696879667,
  0.0214219325754774,
  0.0214856146595837,
  0.0215491175437103,
  0.0216124428079525,
  0.0216755920095483,
  0.0217385666833377,
  0.0218013683422109,
  0.0218639984775441,
  0.0219264585596254,
  0.0219887500380688,
  0.0220508743422187,
  0.0221128328815438,
  0.022174627046021,
  0.022236258206511,
  0.0222977277151227,
  0.0223590369055709,
  0.0224201870935234,
  0.0224811795769409,
  0.0225420156364085,
  0.0226026965354587,
  0.0226632235208875,
  0.0227235978230625,
  0.022783820656224,
  0.0228438932187787,
  0.0229038166935873,
  0.0229635922482444,
  0.0230232210353529,
  0.0230827041927913,
  0.0231420428439758,
  0.0232012380981154,
  0.0232602910504621,
  0.0233192027825551,
  0.0233779743624595,
  0.0234366068449998,
  0.0234951012719883,
  0.0235534586724479,
  0.0236116800628313,
  0.0236697664472338,
  0.0237277188176025,
  0.023785538153941,
  0.0238432254245091,
  0.0239007815860187,
  0.0239582075838253,
  0.0240155043521156,
  0.0240726728140909,
  0.0241297138821471,
  0.0241866284580502,
  0.024243417433109,
  0.0243000816883436,
  0.0243566220946503,
  0.0244130395129642,
  0.0244693347944169,
  0.0245255087804923,
  0.0245815623031787,
  0.0246374961851176,
  0.0246933112397499,
  0.0247490082714593,
  0.0248045880757123,
  0.0248600514391958,
  0.0249153991399521,
  0.0249706319475108,
  0.0250257506230185,
  0.025080755919366,
  0.0251356485813126,
  0.0251904293456085,
  0.0252450989411148,
  0.0252996580889205,
  0.0253541075024583,
  0.0254084478876176,
  0.0254626799428553,
  0.025516804359305,
  0.0255708218208836,
  0.0256247330043961,
  0.0256785385796388,
  0.0257322392095,
  0.0257858355500594,
  0.025839328250685,
  0.0258927179541294,
  0.0259460052966225,
  0.0259991909079647,
  0.0260522754116166,
  0.026105259424788,
  0.0261581435585253,
  0.0262109284177969,
  0.0262636146015774,
  0.0263162027029303,
  0.0263686933090892,
  0.0264210870015373,
  0.0264733843560862,
  0.0265255859429526,
  0.0265776923268339,
  0.0266297040669827,
  0.0266816217172798,
  0.026733445826306,
  0.0267851769374128,
  0.0268368155887916,
  0.026888362313542,
  0.0269398176397387,
  0.0269911820904977,
  0.0270424561840408,
  0.0270936404337596,
  0.0271447353482779,
  0.0271957414315136,
  0.0272466591827389,
  0.0272974890966405,
  0.0273482316633775,
  0.0273988873686397,
  0.027449456693704,
  0.0274999401154902,
  0.027550338106616,
  0.0276006511354511,
  0.02765087966617,
  0.0277010241588047,
  0.027751085069296,
  0.0278010628495439,
  0.027850957947458,
  0.0279007708070058,
  0.0279505018682618,
  0.0280001515674544,
  0.0280497203370128,
  0.0280992086056135,
  0.028148616798225,
  0.0281979453361527,
  0.028247194637083,
  0.0282963651151265,
  0.0283454571808605,
  0.0283944712413708,
  0.0284434077002936,
  0.0284922669578557,
  0.0285410494109145,
  0.0285897554529981,
  0.0286383854743433,
  0.0286869398619349,
  0.0287354189995426,
  0.0287838232677585,
  0.0288321530440338,
  0.0288804087027148,
  0.0289285906150784,
  0.0289766991493673,
  0.0290247346708242,
  0.0290726975417263,
  0.0291205881214181,
  0.0291684067663454,
  0.0292161538300869,
  0.029263829663387,
  0.0293114346141869,
  0.0293589690276564,
  0.029406433246224,
  0.0294538276096075,
  0.0295011524548438,
  0.0295484081163186,
  0.029595594925795,
  0.0296427132124425,
  0.029689763302865,
  0.0297367455211288,
  0.0297836601887898,
  0.0298305076249208,
  0.0298772881461383,
  0.0299240020666283,
  0.0299706496981729,
  0.0300172313501756,
  0.0300637473296863,
  0.0301101979414268,
  0.030156583487815,
  0.0302029042689891,
  0.0302491605828317,
  0.0302953527249935,
  0.0303414809889161,
  0.0303875456658556,
  0.0304335470449047,
  0.0304794854130155,
  0.0305253610550213,
  0.0305711742536585,
  0.030616925289588,
  0.0306626144414165,
  0.0307082419857174,
  0.0307538081970513,
  0.0307993133479868,
  0.0308447577091201,
  0.0308901415490953,
  0.0309354651346236,
  0.0309807287305032,
  0.0310259325996379,
  0.0310710770030561,
  0.0311161621999296,
  0.0311611884475917,
  0.0312061560015558,
  0.0312510651155326,
  0.0312959160414486,
  0.031340709029463,
  0.0313854443279853,
  0.031430122183692,
  0.0314747428415438,
  0.0315193065448019,
  0.0315638135350446,
  0.0316082640521835,
  0.0316526583344794,
  0.0316969966185581,
  0.0317412791394264,
  0.0317855061304867,
  0.0318296778235532,
  0.0318737944488663,
  0.0319178562351077,
  0.0319618634094152,
  0.0320058161973967,
  0.0320497148231454,
  0.0320935595092531,
  0.0321373504768247,
  0.032181087945492,
  0.032224772133427,
  0.032268403257356,
  0.0323119815325724,
  0.0323555071729501,
  0.0323989803909566,
  0.0324424013976657,
  0.0324857704027703,
  0.0325290876145952,
  0.032572353240109,
  0.032615567484937,
  0.0326587305533729,
  0.032701842648391,
  0.0327449039716579,
  0.0327879147235444,
  0.0328308751031372,
  0.0328737853082498,
  0.0329166455354347,
  0.0329594559799936,
  0.0330022168359894,
  0.0330449282962565,
  0.0330875905524119,
  0.0331302037948658,
  0.0331727682128323,
  0.0332152839943397,
  0.0332577513262411,
  0.0333001703942243,
  0.0333425413828221,
  0.0333848644754222,
  0.0334271398542773,
  0.0334693677005146,
  0.0335115481941457,
  0.0335536815140761,
  0.0335957678381145,
  0.0336378073429825,
  0.0336798002043235,
  0.0337217465967119,
  0.0337636466936625,
  0.0338055006676391,
  0.0338473086900635,
  0.0338890709313242,
  0.033930787560785,
  0.033972458746794,
  0.0340140846566914,
  0.0340556654568185,
  0.0340972013125256,
  0.0341386923881805,
  0.0341801388471764,
  0.0342215408519403,
  0.0342628985639403,
  0.0343042121436941,
  0.0343454817507766,
  0.0343867075438276,
  0.0344278896805592,
  0.0344690283177636,
  0.0345101236113208,
  0.0345511757162053,
  0.0345921847864941,
  0.0346331509753735,
  0.0346740744351467,
  0.0347149553172402,
  0.0347557937722115,
  0.0347965899497558,
  0.0348373439987127,
  0.0348780560670734,
  0.0349187263019868,
  0.034959354849767,
  0.0349999418558991,
  0.0350404874650462,
  0.035080991821056,
  0.0351214550669666,
  0.0351618773450133,
  0.0352022587966348,
  0.0352425995624795,
  0.0352828997824113,
  0.035323159595516,
  0.0353633791401072,
  0.0354035585537324,
  0.0354436979731787,
  0.0354837975344788,
  0.0355238573729168,
  0.0355638776230337,
  0.0356038584186336,
  0.0356437998927886,
  0.0356837021778449,
  0.0357235654054283,
  0.0357633897064494,
  0.0358031752111091,
  0.035842922048904,
  0.0358826303486317,
  0.0359223002383961,
  0.0359619318456125,
  0.0360015252970127,
  0.0360410807186503,
  0.0360805982359056,
  0.0361200779734907,
  0.0361595200554543,
  0.0361989246051867,
  0.0362382917454249,
  0.0362776215982569,
  0.036316914285127,
  0.0363561699268402,
  0.0363953886435669,
  0.0364345705548478,
  0.036473715779598,
  0.0365128244361123,
  0.0365518966420688,
  0.0365909325145341,
  0.0366299321699675,
  0.0366688957242252,
  0.0367078232925648,
  0.0367467149896497,
  0.0367855709295533,
  0.0368243912257629,
  0.0368631759911844,
  0.0369019253381463,
  0.0369406393784034,
  0.0369793182231416,
  0.0370179619829813,
  0.0370565707679816,
  0.0370951446876444,
  0.0371336838509182,
  0.0371721883662021,
  0.0371721883662021
};



const double flarge_table[] =
  { 
    243.315981116462,
    243.315981116462,
    23.9600361713695,
    6.9139329716018,
    3.16392425109494,
    1.86371728900425,
    1.27777777777778,
    0.964508403261589,
    0.77581442422356,
    0.651853237448473,
    0.564939170766789,
    0.50087416614881,
    0.451762665758072,
    0.412917778678493,
    0.381404155091771,
    0.355299463329141,
    0.333296455886513,
    0.314477147580053,
    0.298178842729622,
    0.283911535127666,
    0.271305240150892,
    0.260075403532531,
    0.249999578782356,
    0.24090133037795,
    0.232638888888889,
    0.225097003008273,
    0.218180986989316,
    0.211812304114001,
    0.205925243280314,
    0.200464385692282,
    0.195382650833982,
    0.19063977276852,
    0.186201099999877,
    0.182036641362817,
    0.178120300939,
    0.174429259613497,
    0.170943471418511,
    0.16764525048842,
    0.164518930106658,
    0.161550579534251,
    0.158727767472067,
    0.156039363405452,
    0.153475369911586,
    0.151026780420806,
    0.148685458018048,
    0.146444031726265,
    0.144295807386892,
    0.142234690785429,
    0.140255121094728,
    0.13835201304867,
    0.136520706532794,
    0.1347569225002,
    0.133056724301448,
    0.131416483664691,
    0.129832850683331,
    0.128302727268412,
    0.12682324360563,
    0.125391737225632,
    0.12400573435363,
    0.122662933252459,
    0.121361189313573,
    0.12009850168457,
    0.118873001250667,
    0.117682939811998,
    0.116526680319474,
    0.115402688049678,
    0.114309522614568,
    0.113245830714757,
    0.11221033955645,
    0.111201850861809,
    0.1102192354109,
    0.109261428060689,
    0.108327423192858,
    0.107416270547744,
    0.106527071406495,
    0.105658975087763,
    0.104811175728943,
    0.103982909325178,
    0.103173451002242,
    0.102382112501895,
    0.101608239860516,
    0.100851211263824,
    0.100110435062178,
    0.0993853479325371,
    0.0986754131745014,
    0.097980119129101,
    0.0972989777100651,
    0.096631523038292,
    0.0959773101711038,
    0.0953359139186481,
    0.0947069277405102,
    0.0940899627162219,
    0.0934846465839213,
    0.0928906228419223,
    0.0923075499084108,
    0.0917351003348998,
    0.0911729600694444,
    0.0906208277659599,
    0.0900784141362873,
    0.0895454413419325,
    0.0890216424226529,
    0.0885067607592958,
    0.088000549568501,
    0.0875027714270677,
    0.0870131978239609,
    0.0865316087380871,
    0.0860577922401151,
    0.0855915441167475,
    0.0851326675159692,
    0.0846809726119106,
    0.0842362762880613,
    0.0837984018376643,
    0.0833671786802068,
    0.0829424420929991,
    0.0825240329569059,
    0.0821117975153618,
    0.0817055871458615,
    0.0813052581431724,
    0.0809106715135683,
    0.0805216927794322,
    0.0801381917936175,
    0.0797600425630009,
    0.0793871230806953,
    0.0790193151664281,
    0.0786565043146194,
    0.0782985795497303,
    0.0779454332884719,
    0.0775969612084976,
    0.077253062123222,
    0.0769136378624335,
    0.0765785931583867,
    0.0762478355370815,
    0.0759212752144541,
    0.0755988249972193,
    0.0752804001881223,
    0.0749659184953696,
    0.0746552999460257,
    0.0743484668031706,
    0.0740453434866294,
    0.0737458564970926,
    0.0734499343434585,
    0.0731575074732384,
    0.0728685082058719,
    0.0725828706688121,
    0.0723005307362458,
    0.0720214259703205,
    0.0717454955647604,
    0.0714726802907563,
    0.0712029224450229,
    0.070936165799922,
    0.0706723555555556,
    0.0704114382937376,
    0.0701533619337587,
    0.0698980756898617,
    0.0696455300303507,
    0.0693956766382614,
    0.0691484683735212,
    0.0689038592365337,
    0.0686618043331262,
    0.0684222598407987,
    0.0681851829762186,
    0.0679505319639075,
    0.0677182660060694,
    0.0674883452535098,
    0.0672607307776018,
    0.0670353845432538,
    0.0668122693828371,
    0.0665913489710335,
    0.0663725878005655,
    0.066155951158772,
    0.0659414051049953,
    0.0657289164487471,
    0.0655184527286211,
    0.0653099821919232,
    0.0651034737749902,
    0.0648988970841695,
    0.0646962223774351,
    0.0644954205466131,
    0.0642964631001948,
    0.064099322146713,
    0.0639039703786617,
    0.063710381056937,
    0.0635185279957794,
    0.0633283855482,
    0.0631399285918708,
    0.0629531325154621,
    0.062767973205412,
    0.0625844270331088,
    0.0624024708424753,
    0.0622220819379358,
    0.0620432380727561,
    0.0618659174377399,
    0.0616900986502709,
    0.061515760743686,
    0.061342883156971,
    0.0611714457247638,
    0.0610014286676573,
    0.0608328125827897,
    0.0606655784347128,
    0.0604997075465285,
    0.060335181591285,
    0.0601719825836224,
    0.0600100928716606,
    0.0598494951291204,
    0.05969017234767,
    0.0595321078294903,
    0.0593752851800498,
    0.0592196883010845,
    0.0590653013837736,
    0.0589121089021067,
    0.0587600956064349,
    0.0586092465171998,
    0.0584595469188356,
    0.0583109823538382,
    0.058163538616995,
    0.0580172017497721,
    0.0578719580348524,
    0.0577277939908204,
    0.0575846963669894,
    0.0574426521383656,
    0.0573016485007461,
    0.0571616728659458,
    0.0570227128571493,
    0.0568847563043846,
    0.0567477912401137,
    0.0566118058949378,
    0.0564767886934124,
    0.0563427282499708,
    0.05620961336495,
    0.0560774330207186,
    0.0559461763779024,
    0.0558158327717044,
    0.0556863917083178,
    0.0555578428614275,
    0.0554301760687995,
    0.0553033813289536,
    0.0551774487979186,
    0.0550523687860669,
    0.0549281317550259,
    0.0548047283146649,
    0.0546821492201547,
    0.0545603853690978,
    0.0544394277987275,
    0.0543192676831744,
    0.0541998963307965,
    0.0540813051815741,
    0.0539634858045649,
    0.0538464298954196,
    0.0537301292739549,
    0.0536145758817836,
    0.0534997617799989,
    0.0533856791469134,
    0.0532723202758481,
    0.0531596775729743,
    0.0530477435552026,
    0.0529365108481215,
    0.0528259721839814,
    0.0527161203997248,
    0.0526069484350602,
    0.0524984493305793,
    0.0523906162259159,
    0.0522834423579459,
    0.0521769210590265,
    0.052071045755275,
    0.0519658099648841,
    0.0518612072964751,
    0.0517572314474861,
    0.0516538762025956,
    0.0515511354321798,
    0.0514490030908028,
    0.05134747321574,
    0.0512465399255321,
    0.0511461974185697,
    0.0510464399717091,
    0.0509472619389156,
    0.0508486577499364,
    0.0507506219090011,
    0.0506531489935488,
    0.0505562336529823,
    0.0504598706074472,
    0.0503640546466374,
    0.0502687806286237,
    0.0501740434787074,
    0.0500798381882968,
    0.0499861598138068,
    0.0498930034755803,
    0.0498003643568319,
    0.0497082377026125,
    0.0496166188187944,
    0.0495255030710775,
    0.0494348858840139,
    0.0493447627400532,
    0.0492551291786056,
    0.0491659807951242,
    0.0490773132402045,
    0.0489891222187022,
    0.0489014034888677,
    0.0488141528614973,
    0.0487273661991017,
    0.0486410394150891,
    0.0485551684729651,
    0.0484697493855474,
    0.0483847782141954,
    0.048300251068055,
    0.048216164103317,
    0.0481325135224896,
    0.048049295573685,
    0.0479665065499194,
    0.0478841427884249,
    0.0478022006699761,
    0.0477206766182274,
    0.0476395670990636,
    0.0475588686199625,
    0.0474785777293681,
    0.047398691016077,
    0.0473192051086343,
    0.0472401166747416,
    0.0471614224206753,
    0.0470831190907155,
    0.0470052034665852,
    0.0469276723668991,
    0.0468505226466234,
    0.0467737511965439,
    0.0466973549427445,
    0.0466213308460944,
    0.0465456759017447,
    0.0464703871386338,
    0.0463954616190012,
    0.0463208964379101,
    0.0462466887227783,
    0.0461728356329171,
    0.0460993343590784,
    0.0460261821230094,
    0.0459533761770151,
    0.0458809138035283,
    0.045808792314687,
    0.0457370090519191,
    0.0456655613855335,
    0.0455944467143193,
    0.0455236624651509,
    0.0454532060925998,
    0.0453830750785534,
    0.0453132669318397,
    0.0452437791878588,
    0.0451746094082199,
    0.0451057551803846,
    0.0450372141173166,
    0.0449689838571361,
    0.0449010620627813,
    0.0448334464216739,
    0.0447661346453912,
    0.0446991244693435,
    0.0446324136524557,
    0.0445659999768557,
    0.0444998812475661,
    0.0444340552922024,
    0.044368519960675,
    0.0443032731248964,
    0.0442383126784931,
    0.044173636536522,
    0.0441092426351912,
    0.0440451289315857,
    0.0439812934033966,
    0.0439177340486556,
    0.0438544488854725,
    0.043791435951778,
    0.0437286933050696,
    0.0436662190221621,
    0.0436040111989415,
    0.0435420679501227,
    0.0434803874090116,
    0.0434189677272704,
    0.0433578070746858,
    0.0432969036389425,
    0.0432362556253984,
    0.0431758612568642,
    0.0431157187733861,
    0.0430558264320317,
    0.0429961825066796,
    0.0429367852878112,
    0.0428776330823068,
    0.0428187242132439,
    0.0427600570196989,
    0.0427016298565515,
    0.0426434410942925,
    0.0425854891188339,
    0.0425277723313219,
    0.0424702891479532,
    0.0424130379997931,
    0.0423560173325973,
    0.0422992256066352,
    0.0422426612965169,
    0.0421863228910218,
    0.0421302088929301,
    0.0420743178188567,
    0.0420186481990879,
    0.0419631985774193,
    0.0419079675109974,
    0.0418529535701626,
    0.0417981553382947,
    0.0417435714116607,
    0.0416892003992645,
    0.0416350409226989,
    0.041581091616,
    0.0415273511255028,
    0.0414738181096999,
    0.0414204912391014,
    0.0413673691960969,
    0.0413144506748198,
    0.0412617343810132,
    0.0412092190318976,
    0.0411569033560404,
    0.0411047860932278,
    0.0410528659943376,
    0.0410011418212142,
    0.0409496123465455,
    0.040898276353741,
    0.0408471326368122,
    0.0407961800002536,
    0.0407454172589268,
    0.0406948432379447,
    0.0406444567725581,
    0.0405942567080438,
    0.040544241899594,
    0.0404944112122069,
    0.0404447635205797,
    0.0403952977090019,
    0.0403460126712507,
    0.0402969073104878,
    0.0402479805391573,
    0.040199231278885,
    0.0401506584603793,
    0.0401022610233328,
    0.0400540379163262,
    0.0400059880967321,
    0.0399581105306216,
    0.0399104041926707,
    0.0398628680660689,
    0.0398155011424285,
    0.0397683024216952,
    0.0397212709120599,
    0.0396744056298716,
    0.0396277055995515,
    0.039581169853508,
    0.0395347974320531,
    0.0394885873833192,
    0.0394425387631782,
    0.0393966506351601,
    0.0393509220703742,
    0.0393053521474297,
    0.0392599399523587,
    0.0392146845785392,
    0.0391695851266197,
    0.0391246407044446,
    0.0390798504269797,
    0.0390352134162404,
    0.038990728801219,
    0.0389463957178138,
    0.0389022133087593,
    0.0388581807235562,
    0.0388142971184039,
    0.0387705616561318,
    0.0387269735061335,
    0.0386835318443004,
    0.0386402358529565,
    0.0385970847207942,
    0.0385540776428107,
    0.0385112138202453,
    0.0384684924605173,
    0.0384259127771645,
    0.038383473989783,
    0.0383411753239674,
    0.0382990160112515,
    0.0382569952890498,
    0.0382151124006002,
    0.038173366594907,
    0.0381317571266841,
    0.0380902832562998,
    0.0380489442497219,
    0.0380077393784629,
    0.0379666679195267,
    0.0379257291553555,
    0.0378849223737772,
    0.0378442468679537,
    0.0378037019363299,
    0.0377632868825826,
    0.037723001015571,
    0.0376828436492868,
    0.0376428141028059,
    0.0376029117002394,
    0.0375631357706867,
    0.0375234856481875,
    0.0374839606716758,
    0.0374445601849334,
    0.0374052835365444,
    0.0373661300798504,
    0.0373270991729057,
    0.0372881901784334,
    0.0372494024637818,
    0.0372107354008813,
    0.0371721883662021,
    0.0371721883662021
  };

#define FLARGEX 0.002
#define FSMALLX 0.00001
#define FSMALLMULT 250000
#define FLARGELEN 500
#define FSMALLLEN 500
#define FSMALLSTEP 0.000004;

/* FLOOKUP macro. NOTE! 0<=(int x)<=1 
 * Tested u:OK
 */
#define FLOOKUP(x) (((x) >= FLARGEX) ? flarge_table[(int) (1/(x))] : \
                    ( ((x) >= FSMALLX) ? fsmall_table[(int) ((x)*FSMALLMULT)] : 0))

/* Macro to test if FLOOKUP above can be used 
 * Tested u:OK
 */
#ifdef TFRCDEBUG
#define FLOOKUPTEST(x) do { if ( (x) < 0.0 || (x) > 1.0 || ((x)>= FLARGEX && (int) (1/(x)) < 0) || ((x) < FLARGEX && (x) >= FSMALLX && (int) ((x)*FSMALLMULT) < 0)) panic("FLOOKUPTEST failed!"); } while (0)
#else
#define FLOOKUPTEST(x) 
#endif

double tfrc_flookup_reverse(double fvalue);

/*
 * Inverse of the FLOOKUP above
 * args: fvalue - function value to match
 * returns:  p  closest to that value
 * Tested u:OK
 */
double tfrc_flookup_reverse(double fvalue){
  int ctr;
  if(fvalue >= flarge_table[1])
    return 1.0;
  else if (fvalue >= flarge_table[FLARGELEN]){
    ctr = FLARGELEN;
    while(ctr > 1 && fvalue >= flarge_table[ctr])
      ctr--;

    /* round to smallest */
    ctr = ctr + 1;
    
    /* round to nearest */
    /*    if (flarge_table[ctr] - fvalue > fvalue - flarge_table[ctr+1])
          ctr = ctr+1;*/
    return ((double)(1.0/((double) ctr)));
  } else if ( fvalue >= fsmall_table[0]){
    ctr = 0;
    while(ctr < FSMALLLEN+1 && fvalue > fsmall_table[ctr])
      ctr++;
    return ((double)(ctr))*FSMALLSTEP;
  } 
  return TFRC_SMALLEST_P;
}

#endif
