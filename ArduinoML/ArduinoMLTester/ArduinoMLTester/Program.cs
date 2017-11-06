﻿using DataHub.Messages;
using Encog.ML.Data.Basic;
using Encog.Neural.Networks;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace ArduinoMLTester
{
    class Program
    {
        //https://stackoverflow.com/questions/1318933/c-sharp-int-to-byte
        static byte[] GetIntBytes(Int16 intValue)
        {
            byte[] intBytes = BitConverter.GetBytes(intValue);
            if (BitConverter.IsLittleEndian)
                Array.Reverse(intBytes);
            return intBytes.ToArray();
        }

        static byte[] GetFloatBytes(float intValue)
        {
            byte[] intBytes = BitConverter.GetBytes(intValue);
            if (BitConverter.IsLittleEndian)
                Array.Reverse(intBytes);
            return intBytes.ToArray();
        }

        static float[] _weight = new double[] {0.784398753104989,-1.56505396720441,-10.9570750873925,-20.7418127386414,19.4820526461328,23.4484323772917,0.887209742730177,-3.90362483897496,-0.897922870640143,-9.62635812982464,26.0591032935033,1.04549347774238,-24.0496376728272,-5.51705863165609,18.8317900169796,29.8107527896187,14.7856784744295,17.2476441625583,-0.667308065747968,-0.445483472898362,0.907755359834297,-14.6503636768751,0.602878774086458,2.34240691996965,-5.84486399036403,12.3522311952319,-21.0899662351083,0.51075335987874,-0.468925038889801,-0.911632186440204,0.398117635635988,-0.439710168609836,-124.736658331455,-12.679929007421,7.04385168947427,-0.0515630792891982,3.81012651541494,0.522558094939342,15.794048164219,-13.7085890393057,0.747432492348098,-0.800214109684459,1.37752324264769,9.49154641877404,17.5950244591729,-17.5957588421601,-20.6919568644476,0.349214503588087,3.97666633244106,-0.950992411576256,7.38267150968159,-24.1775843603453,-1.35733735839294,21.1663810677411,5.57071554001369,-16.2763122989421,-25.1849790564518,-11.9048983433864,-15.7948974266676,0.753922601205089,-0.638387217427058,0.861351587763957,12.4680805586874,0.430109210103957,-1.60761701611961,4.73031063200434,-11.4425124399421,18.8948429557932,0.82900259344602,-0.532577173315061,1.27435914033326,0.379658539520467,-0.495859319188798,108.537604196414,10.9093006424084,-4.64819092181885,-0.0405725890711107,-2.87077578075036,0.772074241117731,-14.4105968247086,12.8364446079661,-0.715289127021464,0.541008451666205,0.24133756324824,-0.0950574432468539,-0.0587714557128056,-1.19323378469056,-0.862103063827372,0.477221130982425,-0.624284306905421,-0.636471809396922,-0.714628124639012,0.647607070226103,0.0322792013754743,0.173699038281983,0.560091886337748,-0.223208135740581,-0.36449503783915,0.4537198196024,0.157403777399616,-0.864678108760721,0.0137926349135078,0.0741254554700254,-0.508131627859011,-0.571727888276597,1.10660529702237,-0.634872074795476,-0.920834469393821,0.617598464391011,-0.0366030819262975,-0.971360508645591,-0.238571651617972,0.695725771086177,-0.153807752087704,0.328940884291549,-0.861889050003875,-0.908352473938696,2.41666755716055,-0.321191889901842,0.519445136826729,0.203428058518179,0.250088329691291,-0.381879076788607,9.83314317154785,-3.37660779630986,0.275289327333818,1.48493349690391,25.9949012420856,-3.61331898109232,-0.894451980333209,8.65904540279866,1.30666590051045,-1.30334825036814,-0.013692933204407,-1.78337450690743,5.7261408911791,-0.445039714868452,0.922503978414815,-4.48706562505711,13.9517431569389,0.532477017212932,-0.0943432944216479,-1.94249296538807,17.1113746149365,-0.148950837544828,0.088324984822831,1.39732707368601,-3.94560684383413,0.784016962343834,0.762084067919483,7.86239761759323,1.31446372781583,-2.64426920679596,-0.783828628931183,0.43197534449517,-3.99985033172508,-0.0700105443499352,0.596772990406335,-0.14872110506768,-1.11520766898967,-30.599807296059,-0.707052019587936,9.28378026987192,-0.569493979316113,1.53534627039608,-0.747391146543247,-0.114430025443072,-1.4427490012869,24.4401641436445,-0.828713924627572,1.27769311697033,4.19468112807521,1.12976816300596,-0.387676010046602,1.00710076240825,-0.844102251897977,1.07454026765715,0.128909300150038,0.26983226138613,0.365421867456301,5.26371352895348,0.608884124425668,-0.113300369057123,-1.79739667332644,0.481328167214691,-0.296150151585271,-0.409649781203411,-0.729153851297093,2.78644842699672,-0.313253325386488,0.528293802472643,-0.442757054849194,-1.10574059448893,-1.12928065659584,0.751455325317883,1.2796093238093,-0.394635020758169,-4.91576481246123,0.995401794932647,-9.21041540322351,-39.4681708200034,-0.652378809869389,-0.159602272683299,-3.34879030033878,0.316247274342882,3.22743406724297,-0.275181570667416,-0.680147629393716,1.88521711514787,3.28422395674959,-0.146276022193539,-0.812263431129622,2.5634361127936,-3.10871829462815,0.681481131183098,-0.785588670245475,-0.506367856225167,0.920766192918098,2.13608893885287,0.974543334885855,-2.558748907689,3.28245386449318,-1.10786010305479,0.18531208164855,-2.39452511211541,3.24178184587256,0.764249978765484,-0.4796397557919,1.54289117078212,8.21197311322961,-0.772880188877885,0.0957373976720338,12.4768504477231,7.14755422518429,0.407336265715696,0.706730248049208,-0.774231084229571,13.9103458566544,0.822263962960531,0.227603074048221,-7.51465840575335,17.5477895131609,-2.34590889547083,-0.669367057680567,-6.81785142268517,-0.694152164702047,0.812156742396625,-0.019746213011896,0.748806684363677,1.66435817428525,1.03718136194175,-0.862012074852089,-0.784498653384617,-0.412804038389594,-1.38038926105474,-0.949088652073147,0.0252265620973594,-1.06664058032789,0.0689844471267822,0.935440049945552,-0.776324663600358,-0.789794005590926,-0.0641054974531625,-0.317814168350314,-0.563886154019654,-7.192757329267,-3.36573175303931,0.920924750084168,-0.62733469604457,-4.17333686224519,-0.715841113547056,-0.141102374841315,0.373606324429471,4.40233395908628,0.0473767072007049,0.0576548732986666,0.952068722868684,1.6370408763683,-0.60287102169183,0.97832325063311,0.479511510796533,-4.7494162834019,1.81264231306414,1.78224908760463,0.756090209352905,-3.44244003366923,-0.450980928805718,0.139863938956719,-0.293504100942861,0.462620184345854,0.294197513515133,-0.013157181500622,0.517310975749094,-0.589379456612599,-1.25520681806732,0.0544097662742035,-0.621707101666302,0.0136034548834196,-1.74258305219624,-0.981372606972592,-0.875495872311742,0.292806254661875,-1.1273831602532,0.893471227569423,0.0382569425087341,-0.256751618409325,7.16024511838983,0.626465762587968,0.755492257735191,1.13994081538583,15.8189955411703,1.47006057246146,0.520393574337009,0.389094539260643,1.08101790267181,-0.994392395448578,0.485279850846184,-0.215150600117238,-0.0493142714931734,2.56998850382545,-1.39812461192859,0.948136883166827,-6.54980281879791,-2.74251607832667,0.240779484850708,0.300866461207738,-0.273020013227586,0.101368711615908,1.25831776549138,-0.209635463580876,0.996360506434007,0.155675268109128,1.60463256046989,-0.39191310410541,-0.674828657365569,-0.993047276010709,-0.447962936261104,-0.421921539142643,0.152097354150949,-1.78214112393735,0.304043988356624,0.695198248154233,-0.239779515605914,-4.67010440592639,-8.39961152101512,-0.22076236056563,0.56839067102689,-0.7409919506789,-0.50155412332215,0.481358883465541,-0.709801534488551,-10.3237128561789,-1.0291044473019,-0.512959191444397,0.216758149576278,-1.8410154092462,2.73078309558069,0.530248903259036,0.274533737965086,-1.23999270489136,6.29182654449072,0.548692750250745,-0.874373007145954,-1.43401150575112,-0.828669183467317,0.990161903216432,0.152880454156577,-7.617381733446,0.828676264016069,-12.9491968650952,-0.529326693700077,0.26036048906287,0.721295285562843,-0.17936765487111,1.27083468307124,0.149366537329241,-6.0410797662991,1.80388613101276,0.794491446613344,0.976739934799921,-5.18869255200925,-3.27359429988001,3.29314806260589,0.591606799988829,-2.49041355509045,-0.588738949571597,0.453182747544381,-0.763979339076531,-3.76493173130336,-0.0686229591105521,-0.765127888836235,-0.553124615970594,1.22444849510985,2.7667455629149,0.808918320801472,-0.35138952919049,-0.0911495322675936,-6.16557322348365,-0.896085707604145,-0.158651214219524,-1.26860283259437,1.33244362488625,-0.845973761428495,-0.82604520415097,0.238724519233928,-0.909946760327538,-0.692232376133859,0.109077591814407,1.48339130276206,0.288307529130469,-0.958278397126925,0.443909931442408,-0.702848394699428,-1.10721296092625,-0.811075396341692,0.820785563257682,-0.795108153660046,0.653206337553575,0.396589582180455,0.425397755518924,0.22926168588433,-0.45822528277377,0.384130781718958,0.259570984339837,-0.580911408387807,-1.17486930894653,-1.29101082801266,-0.398170568375072,-0.57234362755329,1.07568646615726,0.672665265713687,0.665704738994371,-0.588074017923642,-1.39983111114742,-1.41975866328645,-0.705492982340953,-0.906614580782677,-0.757710308822443,0.437494541995972,0.633512866629352,-0.780257045472813,0.463726720891013,-0.439139971545611,0.121448030326078,-0.772984108064797,-0.741096466165737,0.188239226784806,-0.780667247258906,0.345867712234872,0.630061537889905,0.268016330163544,7.81725840906637,2.14201534319224,-0.605416916977289,0.74952393255385,-2.70892560580838,0.296344843407761,0.554766732916441,3.25828794825107,-3.65030920705569,-1.1357980551715,0.40456283681217,0.48014102724945,0.474155966540619,-0.147819939310779,-0.42742423646121,-1.28282459096362,1.70625056596816,-0.0287405895621977,-0.577800094065392,-4.734988271812,1.80831037281034,0.723224693874085,-1.13627531561101,0.216196734285165,-1.44833946593276,0.412208362138342,0.816068532251514,2.07425643797993,2.29860391711538,0.559884827980297,-0.279852566602928,-2.98818893145298,10.0654174638238,0.460969512463416,-0.245458731667802,-1.62185520019793,4.38956803443317,0.82184866121353,-0.622699876059929,-4.55877245425513,0.722873759985207,-3.13987485362741,0.755245257222037,0.458549539071433,-0.738417925032676,0.538344303756593,0.184844728600567,-0.0227220567337252,0.0062105535858594,2.39686980874076,0.00891262380533357,-0.295594469841897,0.37251138144112,3.22003614736358,-1.32957940833102,-0.891061173374415,-0.197331261642434,-3.37258698695082,-0.271581677175759,1.01626723956862,1.89153470590614,-1.09663584965931,-0.601309750836603,1.11536628575259,-3.96311953934433,0.293743278778274,0.518918435350712,1.07357688532929,-4.05394427761962,2.05566738463181,0.321144202570242,0.172817270234271,-2.73017193673283,-1.70920641580867,-0.654310064922969,0.524569033249714,0.197037191802763,-0.487928973957526,1.32882333769607,-0.468056047418403,1.09378153720435,-0.869652469171451,0.0905439745055364,-2.175012121272,0.0441426977677969,1.18488909989747,3.21079298757593,-0.184638510496844,-1.96839829523609,-2.31935701494347,3.43353079348963,-1.08006192896987,-0.635416155697009,0.480376976692051,0.31935980819679,-0.917496841329035,-1.67019915495337,-2.32387706562389,-0.729098297454687,-0.455541676307475,0.147114293579904,-5.72452261835888,0.249494781030114,-0.212561279836186,-0.793886641718218,-0.782528173252993,3.50431673170086,-1.41550636529077,-0.851659036103764,-0.00404239229681563,-0.892643153686429,-0.245948330059355,-0.631483002843457,3.27994064860673,-1.75103415454483,-0.584532356760549,0.50728940818244,5.02406067653539,12.061164178871,-3.42161822675736,0.866888879122054,2.16483173915371,0.333368964464526,1.65276855509906,-6.37633938081579,-0.750156003752874,-0.791352935180994,-0.367071510344954,-0.96221096107829,0.280207888741853,-0.688525647937592,-1.53559532040552,0.0837257065544446,0.462949597595988,-0.659350599708297,-0.38759747404799,0.740552067212026,0.684945693692835,0.537861194865028,2.71868988828397,-0.754183806397754,0.4572409113316,-1.77512129955305,3.38798091057689,-0.858767855207627,1.02701397693963,1.40331734837454,0.692687143940889,0.468860051771956,0.411106350176728,-2.2010504365032,-3.27909529181083,-0.197319865985927,0.801531742674358,0.771192478788898,-2.35737223514674,-1.01804284939698,-0.32606647068519,8.9197381818863,-0.35806822666533,-2.55575614539688,0.199228343541623,2.91611509654905,0.813628590905816,-1.39056653776789,5.16986237517011,0.659121528869892,-1.35565805144489,-18.9027738753314,0.874104384563155,-0.0441212575285264,12.7574569683293,0.472619566148016,-1.12645581448888,0.649099289250116,4.20505896891753,-1.15993221647741,-0.250500377118604,-0.881868524025952,0.482795689169968,-0.644609646709513,-0.710175432271486,0.542314470743484,-2.87017923736543,-7.68545386268336,-0.106320584954417,-0.767179317764124,0.808482100783251,6.82783957509924,0.300365324359713,0.64524102249337,2.42532620803886,-1.89437041604953,0.755702221366301,-0.674383620039872,-0.109147049909264,4.89063032358792,0.461273039698833,-0.711136658507628,-19.891850531031,1.72870704965209,-0.978037857530711,-0.89295250171417,-4.08890691943866,-0.422063518422217,3.2559346079458,2.15992013009182,0.365427185114593,0.445910956720364,-0.108493636692659,0.629508513286353,0.0902031970402152,1.59090116720566,-5.84559058246469,0.325028226461114,0.281651236296376,7.06010089244218,-1.7954855022883,-0.50498697368545,0.675969613359308,0.266616851759416,0.557048503391138,-0.608094635452245,-0.378487578384828,-5.00626315442885,-2.52591336005721,-1.14768653994211,0.293200616668506,-0.755466561667345,4.92931427046419,-1.22048897666396,0.202657823750924,10.5961220240399,2.45625712358281,-2.32691150618437,1.42120187500111,4.43884499114207,-1.79682028733294,-0.985908058572498,-0.338580211185621,-5.80941416436849,6.39981540146169,-1.85623726644377,2.75370251359302,0.380264770844638,0.543113797484146,-0.729548239879089,0.697001902745656,0.0903660811441456,-0.437084955853212,-2.58074222630267,-0.670955024411091,0.348282551814555,-0.0109853999600476,-1.73050421809041,-0.405643946435018,-0.64648302069545,-1.22115563000173,4.61271465445318,0.441717083019781,0.549103531512605,0.256754015567682,2.14937513060521,-0.35056700734995,0.677995236467718,-6.98376665718994,1.52118863183571,-0.612946496885383,1.00959882791706,6.78356253792388,-2.57502249690557,0.19218774299514,0.618808484308139,-0.190136459459494,-2.02376935863894,0.505943161465618,0.0505974092677121,0.325044187524891,3.47646725366363,-0.792661695936551,0.74787469937647,2.64948752832102,-0.635278071086919,0.263628448026115,-0.242609129844161,0.821202413516774,0.0494474510291041,-1.70184527598455,-1.54794676643075,0.936188090530348,-0.696039347218205,-0.73378989597762,-1.48977195432277,0.213519203258688,0.338490315641163,-1.47670312447747,-9.27779451297918,0.0447163753678268,-2.99536170974473,-3.88460557330957,-6.94526373356275,-2.37467137802887,-0.431861383011643,3.16365857351736,-2.76222827724177,0.393488958484865,2.63381380639616,-1.43061937919364,-20.2962503984598,-0.338716709955028,-4.77533030425534,2.57855004150065,-0.126645909001043,-0.802647726765357,-2.38215761564537,-6.46429584630507,0.0907757331267948,0.75061739257662,-1.53622390676453,3.46573528882496,-1.17916752248448,-0.437117722390609,-0.266624590907016,2.04672636058165,-1.77893703604248,-0.133551380126357,-10.4002501568431,0.146730057842329,-0.16718900256143,0.814815981453954,0.927625849527708,-0.976215643768759,-0.286630569503587,0.460971089349993,0.304867190625026,-3.56501592465868,-0.393874760381892,0.376142189800496,0.230227561243386,-1.03245266762767,-0.664946133253421,-1.44335540974256,0.687131572615299,0.264468971797789,0.685960526442528,0.292724548541208,0.449901564868037,0.812184328198798,0.511632159307265,-1.54727455358152,0.748116501176944,-1.07873336045338,2.87456351473838,-1.98892305317814,0.771635677428642,0.0793344359061913,-0.159483295824068,0.399598861702067,0.682966004254738,2.94503377722335,-7.93388966286819,0.302484183781768,-0.0469949283864325,8.11280094108396,0.831796052732193,-0.532058268058886,0.0449168383560631,0.977296812065724,0.741817215235141,3.52945046512043,-0.649063114310689,0.312842806903576,-1.46882418147346,4.56051369759056,0.209849304736149,0.0864974462591841,2.15925471780485,-3.45878206952962,-0.451375941616471,0.662660604165221,2.33486434410585,-0.098249730240346,1.41892766855207,-0.04025856524336,0.047150479277937,1.14209703619379,0.334158973894508,0.638919553744133,0.461530853689398,0.239462886599757,0.511710932112518,-0.301003253532015,8.70651246294634,-7.26111382249072,0.729331156815084,0.621986522830522,3.27196503700647,10.6224485045604,0.644293862246591,-0.837655105488539,-0.214485825250311,6.62647373454977,-2.14768724463579,-0.731122232710487,-9.79000400402711,2.62281561749715,0.0494850319772391,-1.15540527882171,-4.15840200664857,1.10908833835787,-1.87105459510363,-0.836900668551437,0.71774995486339,-0.428779788044158,0.867415325656443,0.754348481702176,0.768221529382582,-0.268052175563425,3.67636950175019,-0.276934498302281,1.14723816226994,-0.040816850584922,1.07246205995084,0.735742119243009,0.614009389086258,-0.712219452818272,-0.669860755992172,-0.172568808782702,-0.843145248241945,0.971705040123311,-0.862503669025425,-0.0868770170062834,-0.172626800731619,8.72623947970357,-0.718536836291128,-0.964153306923688,-0.0419517839653536,0.23269518603478,0.146246024815613,-0.344159231660938,0.225810911613654,-0.642449836809112,0.921652876185072,-0.10001422946902,0.826384154464535,0.337584521001945,2.26085392363849,-0.552278783399054,-0.391694219423515,-1.98827751764933,0.858984091721667,-0.571615176072269,-0.785392025531713,0.671007974816513,-0.151793390590981,-0.119484993792821,-0.607578441777517,-0.44785285376007,0.153100112383471,-0.168446917684341,0.464724681866833,0.345729794898143,-0.331801924872537,0.622024983684756,-0.246603949119445,0.494083792594165,0.0495891093289462,0.544120065152308,0.811447170342524,-0.677557927388944,3.25283663747181,-0.422322842278337,-0.502525443432393,-0.496784103876623,0.428743699283081,-0.153318189291352,0.514939786063012,0.898882208395497,-0.89045598546264,-1.14745539290498,0.057235843681021,0.397155736932814,-0.152783837660426,0.979253375780466,-0.229957235723917,0.781920995258545,0.267379975938003,0.559497989036404,0.706597827168396,0.660383428970008,0.507040072872004,0.26894385077882,2.36917545795272,1.09873321382332,0.973551182538481,-0.95053408945449,-0.970307739767542,0.60001589932811,0.17553412246239,0.99689017514167,0.408092116436875,2.14634833917305,0.103182878187925,0.174105109248268,0.479723577197741,0.939245468521378,0.941727463823869,-0.70293737002797,0.749936873729335,0.576340250936871,0.203297049258645,1.46162563001036,-2.59009464842152,-0.415979207155477,0.9171069288368,0.844735147056161,0.640904102735142,-0.240180377693265,0.455040471544284,-3.09870689170372,-0.0496765978147128,0.985640539259488,-0.305227384279717,-0.81541869284915,1.518921859957,-0.184899521008704,-0.318885586515528,-0.643957153499899,-2.42388626533003,1.63464458655247,0.122067780105199,-0.940771226213175,0.0890132391058129,0.519983369401126,0.31612733728983,0.602123360361213,-0.712907447261462,0.493884733053083,0.112658380975309,-0.748670818748721,-0.559571426876409,-0.35784086602574,0.347554060117608,-0.117968957502618,0.46262805849804,-0.378410569746783,0.339174611516006,0.19973626610161,0.93564206276546,0.246724351601153,-0.935792297553933,0.453184319246722,0.0139532267072756,-1.53821894500587,-0.747480250404358,-0.694405417164617,3.30650758138005,0.244092999835414,0.627833829878756,0.946875728964477,0.769978558071892,0.759565132781882,-0.313832316644482,-0.653265491344951,2.74802184941501,-0.0295904552895396,-1.10934446111214,0.0217023372728113,-1.82971277802913,0.542260386399308,-0.72834698406221,0.666340293067503,-1.37018222341319,-0.70227665607036,-5.04530249285706,-1.29084500290892,-0.488691607191198,-6.63875475448192,-1.16100364978293,1.36001186498458,-3.27613861863467,-3.84937096659769,-0.881635289635732,-0.464578144867486,0.475893661505712,0.460309909597123,-1.48062440059201,-1.28875307651528,-1.81800992512218,-0.754076188110901,2.01473177541414,-0.266817948255262,-0.254147284399112,-4.48911505570317,-2.29917171741799,-0.0270520439690422,-0.587124422457049,-3.40699028542349,-3.3911667558443,0.0465137910420791,-0.58853480230677,-0.915729602589084,2.48084486684114,-0.783056574089067,0.335832695099518,3.90698669327326,-0.651365618842627,-0.765055497066454,-0.918366757593422,2.26716492445072,-1.15889143734863,-1.54727396463626,0.142187672727186,0.0328941738194934,0.64610123708955,0.54976460066102,-0.738891686095806,-1.16326523028501,-0.240275436908413,0.598732620029695,0.713441890652397,0.509386550511449,3.05634280998178,2.40626351971206,0.764397227608592,0.31316933664341,0.422898800150441,13.6315319048721,-0.0309099799541095,0.00302447206795846,13.8240510564449,0.868553595076537,-4.32080955311894,-0.903570605698553,-0.179082047720082,-0.638948263148294,-0.295694961281635,0.770261866404865,-3.44897906597324,-1.21284592758413,-0.66068675209864,0.187384553399253,-0.387623002461721,-0.277852241258572,-4.34767675086564,-0.557494867410704,-10.0234470919413,-10.8210269085168,0.121307082316471,-0.378769797658865,0.458253952097485,-0.422443537672059,-2.26605446308674,-0.946221661365117,9.66827886923095,0.556257673928563,2.29485293783339,-0.453580531429263,0.311202413296184,-2.50882067955714,2.05005392111059,-0.411090557958213,0.410677777186765,0.261338465467273,-2.04160766847561,-1.34449909537883,0.63245263381119,1.68215988141574,2.07561002437338,-1.17622632180827,0.922908552485707,-1.01006570198703,2.2409949947713,-0.349827102707674,-0.514883377966815,-1.85084917233975,1.17758603632948,-0.00620991111212559,-0.48568619588845,-5.41566707859437,5.60970363119242,0.830940809737309,-0.677067712789473,1.15341027562092,0.539061452229832,0.541572315967677,0.677505104265413,0.792818298332783,0.957314044669506,-0.547723774180299,0.621808131013001,-8.62830646028409,7.87220667496638,-0.590754345928187,0.687925054629645,3.7010337473781,-0.322875811630444,-0.156336977328744,-1.21964332009108,0.0785995035026709,2.57239571609555,-3.98795876606182,-0.0200974145022484,1.29161660133604,1.61505287101972,0.80298410143089,0.248210209696609,-0.349701697690222,-0.551017270409778,6.09100006208271,0.507400282067902,0.90731079964505,0.950230778255177,-0.948936164213715,-0.345085282601768,0.658436385894955,1.39690339253182,-1.94147849772355,0.470341180548655,-0.139119592527696,0.366180860043437,0.623697092318219,1.02843022389103,-0.0831102935692099,-1.60729891880347,-2.28098754890498,-0.729393068748754,0.636341060473481,-0.956636990120595,0.990564505707655,-0.590000833465939,0.59204604255973,0.718734703810174,7.21900747207595,0.958627046206977,0.136340895562178,0.590138557741483,0.62623556653033,-1.44906175595277,0.917798329112869,0.806443491099825,-0.492625034160243,0.587799681304158,0.664841029423924,0.137136019063929,0.7662242561959,0.414103107876303,0.501876868288872,0.776097299499117,-0.300898672907945,4.89549630515743,-0.267369554394906,-0.3545432500859,0.878077959249316,2.47605021875033,0.604707453866508,0.786006362313376,2.90448952169628,1.17638803887597,0.274908964312376,-0.649378507870637,1.57610697617594,1.65516582423862,-0.279704722744759,-0.320736236603106,0.593040124281472,-1.81700937835851,-0.805823950750909,0.702323696108392,0.201407812425123,-0.624788692195901,-0.441603208974461,-0.343824356245299,2.47624403036333,1.74722874121962,-0.521952850938075,0.0613845502253471,0.934966621546554,0.574002285870839,0.576527035278661,0.640837811644374,-0.0884148337350214,-0.885567714378148,-0.0903502934996057,-0.59370430654804,-0.532807718684779,0.0027955273617592,7.91485361406957,-0.208817402559523,-0.401768885261989,-0.176438803582111,2.48395029098543,-0.297981537535631,0.55512436187195,0.500559975351188,0.576254399408064,-0.403379481683954,0.202327646761532,-0.0058005575051153,-0.82364420583191,0.869137921395257,0.531460789384697,-0.0767344343642774,0.891373506077836,0.634203841660625,-0.0725187155892457,-0.706993930389087,-0.131918721024843,-0.923267336759944,0.415303525602075,0.694296524739881,-0.70029544919119,0.407579382655036,-1.0077067685757,0.784989208478533,-3.04171806606542,0.35393621409571,0.702932402156836,0.271212313477295,0.828088502540285,0.687422974417727,-1.02267296924792,0.810316815583586,0.0445085300320622,-2.65984220193881,1.61111723350367,0.342887764995256,-0.984292297356335,-0.957512606650708,0.88680485604391,0.22282097754216,-0.42630761284065,-1.10541958245284,0.116531969538529,-0.802034000042843,0.285126770508219,-3.50859407623978,-0.957785326920032,-0.252214317577024,0.602574883513317,-0.380058378865248,-0.375097852115088,-0.289459524959261,1.9200871179104,2.7835617296992,-0.938650892517967,0.654650520649112,0.534777280515045,-3.07958729728867,-0.637853788183177,-0.446870072821944,-0.356935976230082,-14.1935529662403,0.323600581482163,0.818973095680701,0.154124647882356,12.4943735804752,-4.2864355083526,-0.693392801400882,2.98569373811502,0.209519240076189,0.861572402738159,-0.0734753223115722,-0.574036731822674,0.0987298396357198,0.618107765567011,0.493029265015715,-0.222245891290143,0.592956178562843,1.88876871321922,-0.174512769865384,-0.93675993628057,-0.828702977370749,-0.271598173972597,0.784039693597652,0.432439131058076,-2.15482437251472,-1.35000866161022,-0.0520191882519287,0.608705144048984,-0.114541680542649,-0.412416151397886,0.797024251434673,0.0641423632489577,0.698293063504329,0.346005133680687,-0.0156444003021169,0.85606304056049,-0.735184842971453,3.45786641965054,-0.173855148905472,0.597600606236174,0.391593430724411,-0.715702618906356,-0.77960330567657,0.269972199616753,0.143427400644873,-1.27298731487398,-0.455380345804006,-0.0278618681804849,-0.886119251758895,0.813367740822118,-0.408334655348578,-0.419006107649123,0.667481207389494,-0.661508839758831,0.449263436565653,0.646811191462059,-0.847738203728808,0.506875730186217,-0.760870505347989,0.700346957288911,-0.315497489602633,-0.475015594596077,-0.585143859113319,-0.763970867450618,-0.837696687569747,0.284785011227829,2.51669674996483,-0.491467952009932,0.311413525869665,1.68617824933495,-1.04684928195881,-0.983659684628992,0.805325644659475,0.10843504005652,0.664398329892597,0.43231634109023,-0.64388327772612,0.834476778954391,0.122121103011739,0.857254414405899,0.237277019061649,-0.135211758450426,-1.20672261886246,-0.123512229823418,0.235674074211165,1.01388531736199,-0.559544165001804,0.805297466430319,-0.767559339151568,0.15213263048139,0.307336401561279,0.111283371516248,0.117336256695601,0.880078928279754,-0.0427814309617955,-0.00686903547702997,0.38941120178769,1.15553903149086,-0.877883469974391,-0.692819214553795,-0.341308380629409,1.09705595399054,0.149010999699499,-0.088373150185583,-0.519700034505703,0.146588358149808,1.08681748667673,0.136115087579925,-0.308649041233473,0.944924719318942,-0.537254442765371,-0.328213256834791,0.862550523536839,0.140765292714689,-0.291332485288776,-1.34444845928884,0.566806795952489,0.748971451809393,0.116842609024913,1.20863157012868,0.176544641083145,0.409257458342098,-0.262189941223318,-2.18540392176354,-0.0338634541449205,-0.201218091581616,-0.171852918489603,-0.00739318780421517,0.331934585265777,-0.355386510463091,-2.44975259091938,-0.975282706789738,-2.36459996307597,-1.38989565323342,0.349796307128763,-0.280905825612908,6.63784704713424,0.500537821482841,0.465734468185638,1.42179649393247,-0.13322253763989,1.3424040506341,-0.0786873261138903,-0.873366799084245,26.4523998480357,0.234454759671299,0.47241097805777,-0.221910566980903,15.0631274685906,-0.544904233898041,-0.594092569889163,-16.7317965323115,-0.167778402134811,0.77748466004852,-0.53822149924206,-8.38184820076527,-27.1338301753003,-0.168905350367569,0.349201783182922,-0.787516273743871,-18.3996504031833,0.0244031893624036,-0.492924154348261,-13.5646979532126,-0.182043215342689,0.95901938686797,-1.16102602018532,-0.0752709277367311,10.7794029966449,-1.04657580522445,0.214701353800718,17.8445850873081,-0.509287420549621,13.4024439504129,-0.724557892579707,0.684435859614226,-0.740684327102076,4.9279413328045,0.191425745032168,-2.00859440466291,-1.2627346888049,0.781954683593747,0.201545532740835,-0.106219598351299,2.47804142314427,1.87336587678599,0.439517745208729,-0.680819660673186,-0.773330895386361,7.4925999233058,0.112963042326398,-0.432164546746426,-14.0860519318318,-0.868608541975204,0.254004311884979,-0.596266051769623,1.37002336716763,-1.05648915433604,-2.59893270456666,-0.677925610741231,-0.706918285075378,3.11876658425476,0.822042518607413,-0.133001216114754,2.09800791047205,0.469769592162278,1.03395146861485,-0.199455801336217,0.403122347034698,-5.03422576340272,0.948944780388029,0.613997980487436,-1.9657045707674,0.448626121555462,0.638406696641919,-1.76548643101989,0.712033234003904,-0.854817277300856,23.7764200590068,-0.805604524234244,-0.373256458736884,-0.291993388403728,-3.68615697497055,-0.966505737855883,-0.114889132535519,-0.128522614443076,0.394255767895037,0.254005512170632,0.0944354492845973,-0.768591755429978,16.7445624981682,0.697524346723533,0.166629100693312,-1.49578442732041,-121.522880834533,0.033826429551381,0.463970572499361,0.900822539589524,0.610255909577237,-1.04687027158028,0.773900853729691,-0.480074800705517,-3.2725599602927,0.773562611656102,-0.202275548128683,-1.59432191133545,0.82312037384516,-3.07341719553194,0.195686125050753,0.229770241000538,-18.3526783492249,120.366345077012,0.346306154724514,-0.150500000765164,0.21525002771254,2.43249375245991,-1.74627874289294,0.941233893482696,2.74522670868683,-7.16173474423027,-0.639050700587391,0.501093958893162,-9.59665017806516,-2.05743685465956,0.84671957557806,-0.784002164623733,-5.58578805702169,-3.18625014961363,-1.1854622289807,-0.0110263224223054,2.86722882852191,33.9923544487538,1.20705670110103,-0.12653601191017,2.81074042743939,8.07668139372346,0.617906783827276,-0.722469720507297,4.588614277161,8.55537842999642,0.591032985089244,0.527163734120609,0.603362976632397,2.67737861798237,-1.09843627153211,-0.408498767499114,10.8079599132512,-6.20084165890994,-0.790955841302022,-0.0210565493373169,-4.77725250123162,-2.64566007292104,-1.09047699138532,-0.354434397698023,-3.72103763424795,-0.875009643252667,-0.427982725301702,1.55517452796263,0.637781644438532,-0.766171394691028,0.67900029005512,-0.996433806710044,-0.106873531640465,0.147136981252143,0.818817045076498,-0.685564877905863,0.158883243482608,0.701547378271928,1.7643092169922,0.758429371417521,0.617514130636851,0.0694460185590124,-0.884941523969322,0.0261882154093371,-0.109855102570282,0.440800577566384,0.686986347699976,-0.387323459374663,0.701668584408156,-2.84214784831831,-0.574757037446018,0.428799685174459,-0.471599836160673,-0.893574440819413,0.478446665520041,-0.495255094682349,-0.1196544148471,0.0685049107424472,-0.0365071190626878,0.0581972964189599,-0.150756446283438,-0.542337159265512,-0.804305582998669,-0.669958526371394,-0.899164424424111,1.34601569440351,0.0850159704301355,1.39670494304397,-2.44016166316227,-0.0792886171968397,-0.561683213274766,0.406803773952843,0.718157303943028,0.0280058826515378,-9.15834019003171,0.468529990776949,0.450244344699656,0.851578001559139,-5.42050762802668,5.49573082431381,-0.765515746815356,0.89064052874219,20.3459115396533,4.29990210931928,0.419085309304756,-0.512437219762211,2.43665686912041,-0.25670148918502,-0.646184524274835,0.00560967106927028,-5.14024782756964,0.282771084510277,-1.48980478238196,-0.569024816609287,-2.4621007127568,2.07180430215959,-1.82112690058682,0.180366409328965,0.934922075390776,-5.25008244702714,-0.263132858929515,0.975959747938962,-2.91152954363972,2.05954432642004,3.8817069490036,0.953894952155005,-6.2599589710898,-0.722078155376763,0.922788327304073,1.00202701306218,0.182523822186035,-3.42438139899067,-0.710891395491746,0.233517710548724,1.89170203576935,0.924264540317802,3.10417788846237,1.04954606952637,-0.230316717985449,0.032737519566236,-6.51024354059748,0.853292286536288,1.19663962132754,3.59206457744023,-8.33512017579489,-0.102869003194234,0.762187223334045,0.216431320717141,-3.15579915701005,-0.0654706592656042,-0.317050157627708,2.48562381202216,-1.50667534362347,0.790615349473448,0.124648392253411,2.71207005489199,-0.88933721919878,0.0993793745406954,-1.11289085859528,-5.01159835504425,5.37553188923821,-0.627711653407879,0.577395154048383,-20.8147484945813,-1.32872252359085,3.47230543223122,0.203324433689061,-23.9965764108409,0.282468689033146}.Select(f => (float)f).ToArray();


        static void Main(string[] args)
        {
            if (!File.Exists("data.json"))
            {
                HttpClient client = new HttpClient();
                client.BaseAddress = new Uri("http://p5datahub.azurewebsites.net");
                var tests = client.GetAsync("/api/model/128/test/120");
                string json = tests.Result.Content.ReadAsStringAsync().Result;
                File.WriteAllText("data.json", json);
            }
            
            var response = Newtonsoft.Json.JsonConvert.DeserializeObject<Response<TestInfo>>(File.ReadAllText("data.json"));
            var port = new System.IO.Ports.SerialPort(System.IO.Ports.SerialPort.GetPortNames()[0]);
            port.BaudRate = 57600;
            port.Open();

            var network = (BasicNetwork)Encog.Persist.EncogDirectoryPersistence.LoadObject(new FileInfo("Test-120-Model-128-encog.txt"));
            Network network2;
            network2.layerCount = 3;
            network2.weight = _weight;
            network2.offset = new int[2] { 82, 0 };
            network2.neuronCount = new int[3] { 40, 40, 2 };
            network2.bias = new int[3] { 1, 1, 0 };
            DateTime t0 = DateTime.Now;
            Random rnd = new Random();
            if (response.Success)
            {
                float[] totalConfidence = new float[2];
                foreach (var set in response.Data.TestSet.OrderBy(r => rnd.NextDouble()))
                {
                    
                    foreach (var data in set.Data)
                    {
                        List<double> input = new List<double>();
                        List<double> inputa = new List<double>();
                        foreach (var point in data.Data.Take(10))
                        {
                            foreach (var val in new double[] { point.X / 2000.0, point.Y / 2000.0, point.Z / 2000.0, point.RX / 10000.0 }.Select(f => (float)f))
                            {
                                port.Write(GetFloatBytes(val).Reverse().ToArray(), 0, 4);
                                input.Add(val);
                            }
                        }

                        //var output = network.Compute(new BasicMLData(input.ToArray()));
                        //float[] out2 = new float[2], out3 = new float[2];
                        ////compute(network2, input.Select(d => (float)d).ToArray(), out2);
                        //int i = 0;

                        float[] confidence = new float[2];

                        for (int i = 0; i < response.Data.Labels.Length; i++)
                        {
                            while (port.BytesToRead < 4) ;
                            //confidence[i] = float.Parse(port.ReadLine().Replace(".", ","));
                            byte[] floatBytes = new byte[4];
                            for (int b = 0; b < 4; b++)
                            {
                                floatBytes[b] = (byte)port.ReadByte();
                            }
                            confidence[i] = BitConverter.ToSingle(floatBytes, 0);
                            totalConfidence[i] = 0.95f * totalConfidence[i] + 0.05f * confidence[i];
                        }



                        //i = 0;

                        //foreach (var label in response.Data.Labels)
                        //{
                        //    conf[i] = conf[i] * 0.95 + out3[i] * 0.05;
                        //}

                        //var bestLabel = response.Data.Labels[conf.TakeWhile(c => c != conf.Max()).Count()];
                        //var bestLabelEg = response.Data.Labels[confEg.TakeWhile(c => c != confEg.Max()).Count()];

                        Console.WriteLine($"{set.Name};{DateTime.Now.Subtract(t0).TotalMilliseconds};{totalConfidence[0]};{totalConfidence[1]}");
                    }
                    //Console.WriteLine($"{set.Name}: {string.Join(" ", conf)}");
                }
            }
            Console.ReadLine();
        }

        public struct Network
        {
            public int layerCount;
            public float[] weight;
            public int[] offset;
            public int[] neuronCount;
            public int[] bias;
        }

        static float sigmoid(float x)
        {
            return 1 / (1 + (float)Math.Exp(-x));
        }

        public static void compute(Network network, float[] input, float[] output)
        {
            //Initialize arrays to hold input and output values during layer computation.
            int maxNeurons = 0;
            for (int layer = 0; layer < network.layerCount; layer++)
            {
                maxNeurons = Math.Max(maxNeurons, network.neuronCount[layer] + 1);
            }
            float[] tempIn = new float[maxNeurons], tempOut = new float[maxNeurons];

            //Initialize input
            for (int i = 0; i < network.neuronCount[0]; i++)
            {
                tempIn[i] = input[i];
            }
            tempIn[network.neuronCount[0]] = network.bias[0];

            //Calculate output for each layer
            for (int layer = 0; layer < network.layerCount - 1; layer++)
            {
                int offsetIndex = network.offset[layer];
                int fromCount = network.neuronCount[layer] + network.bias[layer];
                int toCount = network.neuronCount[layer + 1];

                //For each neuron in the next layer
                for (int toNeuron = 0; toNeuron < toCount; toNeuron++)
                {
                    //Calculate weighted sum from the current layer
                    float weightedSum = 0;
                    for (int fromNeuron = 0; fromNeuron < fromCount; fromNeuron++)
                    {
                        weightedSum += network.weight[toNeuron * fromCount + fromNeuron + offsetIndex] * tempIn[fromNeuron];
                    }

                    //Apply activation function
                    tempOut[toNeuron] = sigmoid(weightedSum);
                }

                //Prepare input for next layer
                for (int toNeuron = 0; toNeuron < toCount; toNeuron++)
                {
                    tempIn[toNeuron] = tempOut[toNeuron];
                }
                tempIn[toCount] = network.bias[layer + 1];
            }

            //Output from last layer
            for (int i = 0; i < network.neuronCount[network.layerCount - 1]; i++)
            {
                output[i] = tempOut[i];
            }
        }
    }
}