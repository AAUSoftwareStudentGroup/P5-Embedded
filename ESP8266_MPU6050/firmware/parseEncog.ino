#include "parseEncog.h"

bool isNumeric(int c) {
	switch(c) {
		case '-':
		case '.':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return true;
	}
	return false;
}

bool _readInt(File* fp, int* out) {
	String s = "";
	int c;
	while(isNumeric(c = fp->read())) {
		s += ((char)c);
 	}
 	if(s.length() == 0)
 		return false;
 	*out = s.toInt();
 	return true;
}

bool _readDouble(File* fp, double* out) {
	String s = "";
	int c;
	while(isNumeric(c = fp->read())) {
		s += ((char)c);
 	}
 	if(s.length() == 0)
 		return false;
 	*out = atof(s.c_str());
 	return true;
}

void printWeights(network* ann) {
	int n_weights, n_nodes;
	int weightIndex;
	layer* l;
	node* n;
	Serial.print("weights=");
	for(l = ann->layers+ann->n_layers-2; l >= ann->layers; l--) {
		// number of weights is the number of nodes in the next layer
		n_weights = (l+1)->n_nodes;
		n_nodes = l->n_nodes;

		// itterate through the number of nodes in the next layer
		for(weightIndex = 0; weightIndex < n_weights; weightIndex++) {
			// for each node in the current layer
			for(n = l->nodes; n < l->nodes+n_nodes; n++) {
				// read a weight from the file
				// fscanf(fp, "%lf,", &(n->weights[weightIndex]) );
				// _readDouble(fp, &(n->weights[weightIndex]));
				Serial.print(n->weights[weightIndex],16);
				Serial.print(",");
			}

			// plus the bias node
			// fscanf(fp, "%lf,", &(l->bias.weights[weightIndex]) );
			// _readDouble(fp, &(l->bias.weights[weightIndex]));
			Serial.print(l->bias.weights[weightIndex],16);
			Serial.print(",");
		}
	}
	Serial.println();
}

// set file pointer `fp` to the position right after the seeked string
int _parseEncogModel_seekString(File* fp, char* searchString) {
	// rewind(fp);
	fp->seek(0, SeekSet);

	int c;
	char* comparisonRefference = searchString;
	int found;
	while((c = fp->read()) != -1) {
		// if we match a charecter
		if(c == *comparisonRefference) {
			if(*(++comparisonRefference) == '\0') {
				return 1;
			}
		} 
		// else: reset comparisonRefference to start
		else 
			comparisonRefference = searchString;
	}

	// while(feof(fp) == 0) {
	// 	if(fgetc(fp) == *comparisonRefference) {
	// 		if(*(++comparisonRefference) == '\0') {
	// 			return 1;
	// 		}
	// 	} else comparisonRefference = searchString;
	// }

	return 0;
}

void _parseEncogModel_parseNumberOfLayers(File* fp, network* n) {
	n->n_layers = 0;

	int tmp;
	_parseEncogModel_seekString(fp, "layerFeedCounts=");
	// while(fscanf(fp, "%d,", &tmp) > 0)
	while(_readInt(fp, &tmp))
		n->n_layers++;
	n->layers = (layer*)malloc(sizeof(layer) * n->n_layers);
}

void _parseEncogModel_parseLayerSizes(File* fp, network* n) {
	int n_nodes;
	int layerIndex = n->n_layers-1; // start from last layer

	_parseEncogModel_seekString(fp, "layerFeedCounts=");
	// while(fscanf(fp, "%d,", &n_nodes) > 0) {
	while(_readInt(fp, &n_nodes) > 0) {
		n->layers[layerIndex].n_nodes = n_nodes;
		n->layers[layerIndex].nodes = (node*)malloc(sizeof(node)*n_nodes);
		layerIndex--;
	}
}

void _parseEncogModel_allocateWeightArrays(network* ann) {
	int n_weights;
	layer* l;
	node* n;

	// for each layer except output
	for(l = ann->layers; l < ann->layers+ann->n_layers-1; l++) {
		// number of weights is the number of nodes in the next layer
		n_weights = (l+1)->n_nodes;

		l->bias.weights = (double*)malloc(sizeof(double) * n_weights);
		for(n = l->nodes; n < l->nodes+l->n_nodes; n++) {
			n->weights = (double*)malloc(sizeof(double) * n_weights);
		}
	}

	// handle output layer
	l->bias.weights = NULL;
	for(n = l->nodes; n < l->nodes+l->n_nodes; n++) {
		n->weights = NULL;
	}
}


void _parseEncogModel_parseWeights(File* fp, network* ann) {
	int n_weights, n_nodes;
	int weightIndex;
	layer* l;
	node* n;

	_parseEncogModel_seekString(fp, "weights=");

	// for each layer except output (output has no outgoing weights)
	for(l = ann->layers+ann->n_layers-2; l >= ann->layers; l--) {
		// number of weights is the number of nodes in the next layer
		n_weights = (l+1)->n_nodes;
		n_nodes = l->n_nodes;

		// itterate through the number of nodes in the next layer
		for(weightIndex = 0; weightIndex < n_weights; weightIndex++) {
			// for each node in the current layer
			for(n = l->nodes; n < l->nodes+n_nodes; n++) {
				// read a weight from the file
				// fscanf(fp, "%lf,", &(n->weights[weightIndex]) );
				_readDouble(fp, &(n->weights[weightIndex]));
			}

			// plus the bias node
			// fscanf(fp, "%lf,", &(l->bias.weights[weightIndex]) );
			_readDouble(fp, &(l->bias.weights[weightIndex]));
		}
	}
}

network parseEncogModel(char* filePath) {
	// FILE* fp = fopen(filePath, "r");
	File f = SPIFFS.open(filePath, "r");
	network n;

	_parseEncogModel_parseNumberOfLayers(&f, &n);
	_parseEncogModel_parseLayerSizes(&f, &n);
	
	// allocate results array
	n.lastResult.resultLength = n.layers[n.n_layers-1].n_nodes;
	n.lastResult.results = (double*)malloc(sizeof(double)*n.lastResult.resultLength);

	_parseEncogModel_allocateWeightArrays(&n);
	_parseEncogModel_parseWeights(&f, &n);

	return n;

}

void parseEncogModel_setup() {
	File f;
	if(!SPIFFS.begin()) {
		SPIFFS.format();
		SPIFFS.begin();
	}
	if(!SPIFFS.exists("/model")) {
		f = SPIFFS.open("/model", "w");
		// Write default model to file
		// Morten vs grynte
		f.println("encog,BasicNetwork,java,3.4.0,1,63644976912205\n[BASIC]\n[BASIC:PARAMS]\n[BASIC:NETWORK]\nbeginTraining=0\nconnectionLimit=0\ncontextTargetOffset=0,0,0,0,0,0,0\ncontextTargetSize=0,0,0,0,0,0,0\nendTraining=6\nhasContext=f\ninputCount=40\nlayerCounts=2,5,5,5,5,5,41\nlayerFeedCounts=2,4,4,4,4,4,40\nlayerContextCount=0,0,0,0,0,0,0\nlayerIndex=0,2,7,12,17,22,27\noutput=0.562563282531989,0.437822192630827,0.632317542511967,0.625681581174168,0.39864761248837,0.949700849580177,1,0.632578617795363,0.000605920277472321,0.00151056008861802,0.0648917030849507,1,1,1,1,0.00000000000000000001,1,1,0.00000000000000000001,0.00000000000000000001,1,1,1,0.00000000000000000001,0.00000000000000000001,1,1,0.947575,-0.79405,-0.718125,0.40033,0.652125,-0.8521,-0.68795,0.18781,0.233475,-0.85685,-0.624625,0.164865,-0.25105,-1.074575,-0.58485,0.00391,-1.1623,-1.52275,-0.287025,-1.807495,-2.176575,1.15685,0.589,-2.91,-0.67865,1.455475,-0.2398,1.18843,0.370575,0.593275,-0.89345,0.68503,0.6331,0.1503,-0.650175,1.75422,1.362875,-1.299725,-0.66045,0.635935,1\noutputCount=2\nweightIndex=0,10,30,50,70,90,254\nweights=0.7808950693133,1.59502340223518,-1.13509393090671,-0.947516285984316,0.112180672269858,1.16427450380989,-1.19896508580275,1.10908299284042,-0.220855026177495,-0.468413211569832,-0.328425220140501,0.0737711429140736,272.872333506606,1.21267533136925,0.258999478792086,-0.33024001976082,-1.50704772290098,-1.82439299498454,-1.75327831321958,0.840079759980813,-1.21256128108946,1.36786601418686,664.740909008403,2.83730113792475,-0.833140116058751,0.231923759304524,1744.94207210093,1149.11686924838,1.73950884993856,-0.114536714118585,832.735029366382,-833.240129755856,0.222174380843384,1871.13232867456,0.82622114862384,-7.1260789199869,-0.504511437646226,0.880201244293183,-0.168355869586219,-0.65776691861403,-5.52959276710668,-1.75577747794921,-0.661467711756428,-1517.89933956872,1.45307488163267,3.61640508757058,-4.7386441283454,-0.796337856256118,7.66482884316333,-0.749365677116641,9062.88084117971,2168.57872899834,-4709.79216119777,2329.79771093189,0.602950076086468,1695.39868550822,-1367.30961620773,-2378.06962400064,-10.9852523797882,5.60897893242939,1812.22541528673,-555.443711566134,-7100.87293463288,220.960441719196,5.83584291407352,-5927.74395658444,5628.96541215721,11228.9002740586,-19.3597595828691,1.00019255806049,3502.02485006294,-331.805591312141,-5304.96852801399,-1901.55831819073,27.1629354336226,-3102.84455455229,3993.96466304802,12399.0690169781,-4507.60470711467,1048.40073184883,-7318.38615297099,3582.6793906508,8210.30179082712,-2485.52560290409,-1.45691764343282,9133.25202398519,-3574.44081343794,-18317.4954444328,-135.164866610819,-3.57453378762576,9711.87683375299,9967.80765170153,-8299.79771213523,5134.81378119369,5520.92585559527,10724.2596580886,-11797.978123225,3737.39120862601,-11380.0859819646,938.397310211482,-12457.9193024524,8202.98370747032,-8972.31060842846,4701.57429307552,-9182.27323063559,6254.27648605652,-5926.67862575361,9266.60124034512,-6648.89060837801,-5033.09943242947,-9200.82149087938,7501.64202538254,-9776.4586942377,-5826.50677825574,-9933.51171274747,8232.89954965388,-13679.5246573531,-10774.2385762838,-9717.81070272231,7166.74149166144,-11549.4383135341,-9041.87601971778,-13769.3778053734,7149.84526939378,-7250.05194524239,5502.48024588247,-2213.89823119539,7513.63498310629,-8973.99196415964,2286.74915366001,-0.567193150948933,-8074.1730904805,-9703.4777113039,6022.43528840879,-2965.58727483425,-3426.66881679522,-9816.25427804351,12660.8428851352,-4632.90266027284,9037.73651584126,203.419146134051,15204.2558026187,-7437.39410536635,7345.38734374771,-2659.67592427768,9175.88278160908,-7025.88439514427,6742.09807385687,-7835.11510149221,7808.21004909035,4283.80319823013,8504.78449400956,-7245.44712427733,9588.59167097389,8968.71943996079,9550.67441158482,-8185.82155009162,11049.2965697286,11457.853269838,9446.20255349892,-7738.05765302306,11797.9625566582,8500.6728913305,13834.4153772219,-7334.79531305551,7128.37904896767,-6352.36083487378,976.889363755336,-6995.45783214274,8530.56602352225,-3418.59755785796,-3.07179693958655,-7659.49460299703,-9615.57102530621,8279.71685692369,-6059.43118110134,-7079.75088886203,-6705.98943408592,15431.0661553005,-5968.0947569319,8923.75250183847,-840.664943039471,16403.0650761066,-9505.60504080822,8049.0987656295,-1887.98968795676,13356.1090870872,-4553.51621095434,7913.98989225122,-5328.44631740083,11257.4806821267,9408.41966154677,5725.28039516849,-5851.41185753674,9728.26574665054,9219.82864426867,7409.9111281902,-6165.34492918801,9071.64959821315,13716.7145634833,8471.95246049708,-4489.69203405785,10608.7867336519,6013.52907173481,10318.109547449,-3870.94985680266,6610.15086443235,-1010.58571186167,8278.21932045192,-4197.75129046614,6837.67354215604,58.4050531562152,-4.60324055283142,-7894.01462434576,-11218.9350881669,10839.1895801315,1585.53895437912,3659.97768677771,-12529.188012419,-11616.0852164786,-11452.4830900988,10080.200427342,-16382.624592742,-12797.5074890453,-9713.08388360394,8486.7931604733,-14507.9445413284,-4372.75251479002,-17069.9563157625,-12792.1428921229,-10283.8605054239,-2732.21396792226,-27917.0772593167,12419.0315408464,-10117.2083842568,9551.41045308015,-24414.8356955455,2198.44335311652,-6992.98945346399,8549.63671508391,-8826.01109003548,12361.1644361715,-10715.496965322,6904.83512812095,1159.33086592816,7770.41709719425,-11054.6443775565,12518.380692375,4408.35734873763,8987.4850014442,-9901.86095167349,11235.2992641636,14606.3961482732,0.595595217838957\nbiasActivation=0,1,1,1,1,1,1\n[BASIC:ACTIVATION]\n\"ActivationSigmoid\"\n\"ActivationSigmoid\"\n\"ActivationSigmoid\"\n\"ActivationSigmoid\"\n\"ActivationSigmoid\"\n\"ActivationSigmoid\"\n\"ActivationLinear\"");
		f.close();
	}
}
