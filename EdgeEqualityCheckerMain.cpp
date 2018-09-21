/*
 * EdgeEqualityCheckerMain.cpp
 *
 *  Created on: Sep 11, 2018
 *      Author: iqbal
 */

#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <string>

enum MatchType{
	FOUND_EXACT,
	FOUND_REVERSE,
	FOUND_DIFFERENT,
	NOT_FOUND
};

enum DirectionType{
	FORWARD,
	REVERSE,
};
using EdgeMap = std::map<std::string, std::map<std::string, struct OverlapInfo> >;

class OverlapInfo{

public:


	int sourceOverlapStart;
	int sourceOverlapEnd;
	int sourceLength;
	int destinationOverlapStart;
	int destinationOverlapEnd;
	int destinationLength;
	bool orientation;
	int mismatch;

	MatchType matchType = NOT_FOUND;

	//friend bool operator==(const OverlapInfo& oi1, const OverlapInfo& oi2);

};

bool isEqual(const OverlapInfo& oi1, const OverlapInfo& oi2, DirectionType directionType){

	if(directionType == FORWARD){
		return (oi1.sourceOverlapStart == oi2.sourceOverlapStart &&
			oi1.sourceOverlapEnd == oi2.sourceOverlapEnd &&
			oi1.sourceLength == oi2.sourceLength &&
			oi1.destinationOverlapStart == oi2.destinationOverlapStart &&
			oi1.destinationOverlapEnd == oi2.destinationOverlapEnd &&
			oi1.destinationLength == oi2.destinationLength &&
			oi1.orientation == oi2.orientation &&
			oi1.mismatch == oi2.mismatch);
	}
	else{
		return (oi1.sourceOverlapStart == oi2.destinationOverlapStart &&
			oi1.sourceOverlapEnd == oi2.destinationOverlapEnd &&
			oi1.sourceLength == oi2.destinationLength &&
			oi1.destinationOverlapStart == oi2.sourceOverlapStart &&
			oi1.destinationOverlapEnd == oi2.sourceOverlapEnd &&
			oi1.destinationLength == oi2.sourceLength &&
			oi1.orientation == oi2.orientation &&
			oi1.mismatch == oi2.mismatch);

	}

}

void checkEqualityInSga(EdgeMap& edgeMapSga, std::string& sourceVertexId,
						std::string& destinationVertexID,  OverlapInfo& sofOverlapInfo){

	if(edgeMapSga.find(sourceVertexId) != edgeMapSga.end()
			&& edgeMapSga[sourceVertexId].find(destinationVertexID) != edgeMapSga[sourceVertexId].end()){

		//printf("edge found\n");
		OverlapInfo& sgaOverlapInfo = edgeMapSga[sourceVertexId][destinationVertexID];
		if( isEqual(sgaOverlapInfo, sofOverlapInfo, FORWARD)){
			//printf("exact found\n");
			sgaOverlapInfo.matchType = FOUND_EXACT;
			sofOverlapInfo.matchType = FOUND_EXACT;
		}else{
			sgaOverlapInfo.matchType = FOUND_DIFFERENT;
			sofOverlapInfo.matchType = FOUND_DIFFERENT;
		}

	}else if(edgeMapSga.find(destinationVertexID) != edgeMapSga.end()
			&& edgeMapSga[destinationVertexID].find(sourceVertexId) != edgeMapSga[destinationVertexID].end()){

		//printf("reverse found\n");
		OverlapInfo& sgaOverlapInfo = edgeMapSga[destinationVertexID][sourceVertexId];
		if( isEqual(sgaOverlapInfo, sofOverlapInfo, REVERSE)){
			sgaOverlapInfo.matchType = FOUND_REVERSE;
			sofOverlapInfo.matchType = FOUND_REVERSE;
		}else{
			sgaOverlapInfo.matchType = FOUND_DIFFERENT;
			sofOverlapInfo.matchType = FOUND_DIFFERENT;

		}
	}
	else{
		//printf("nothing found\n");
		sofOverlapInfo.matchType = NOT_FOUND;
	}
}

void writeFileWithInfo(std::ifstream& inFile, EdgeMap& edgeMap, std::ofstream& outFile){

	int edgeCounter = 0;
	while(inFile){
		std::string line, tag;
		std::getline(inFile, line);

		std::stringstream ss;
		ss<<line;
		ss>>tag;

//		printf("inside Loop\n");
//		break;

		if(tag == "ED"){
			OverlapInfo ov;
			std::string sourceVertexId, destinationVertexId;

			ss>>sourceVertexId>>destinationVertexId;
			MatchType matchType = edgeMap[sourceVertexId][destinationVertexId].matchType;

			switch(matchType){
				case FOUND_EXACT:
					outFile<<line<<" "<<"(f)"<<'\n';
					break;
				case FOUND_REVERSE:
					outFile<<line<<" "<<"(rf)"<<'\n';
					break;
				case FOUND_DIFFERENT:
					outFile<<line<<" "<<"(df)"<<'\n';
					break;
				case NOT_FOUND:
					outFile<<line<<" "<<"-----"<<'\n';
					break;
			}

			edgeCounter++;
		}
		else{
			outFile<<line<<'\n';
		}
	}

	outFile<<"Total number of edges: "<<edgeCounter<<'\n';

}

int main(){


	std::freopen("inputFileNames.txt","r", stdin);
	std::string sgaFileName, sofFileName;
	std::cin>>sgaFileName>>sofFileName;

	std::ifstream sgaFile(sgaFileName);
	std::ifstream sofFile(sofFileName);

	EdgeMap edgeMapSga;
	EdgeMap edgeMapSof;

	while(sgaFile){
		std::string line, tag, sourceVertexId, destinationVertexId;
		std::getline(sgaFile, line);

		std::stringstream ss;
		ss<<line;
		ss>>tag;

		if(tag == "ED"){
			OverlapInfo ov;
			ss>>sourceVertexId>>destinationVertexId>>ov.sourceOverlapStart>>ov.sourceOverlapEnd>>ov.sourceLength>>
			ov.destinationOverlapStart>>ov.destinationOverlapEnd>>ov.destinationLength>>ov.orientation>>ov.mismatch;

//			std::cout<<sourceVertexId<<" "<<destinationVertexId<<" "<<ov.sourceOverlapStart<<" "<<ov.sourceOverlapEnd<<" "
//					<<ov.sourceLength<<" "<<ov.destinationOverlapStart<<" "<<ov.destinationOverlapEnd<<" "<<
//					ov.destinationLength<<" "<<ov.orientation<<" "<<ov.mismatch<<'\n';
//			break;

			edgeMapSga[sourceVertexId][destinationVertexId]= ov;

			//std::cout<<edgeMapSof[sourceVertexId][destinationVertexId].sourceLength<<'\n';break;

		}
	}

	while(sofFile){
		std::string line, tag, sourceVertexId, destinationVertexId;
		getline(sofFile, line);

		std::stringstream ss;
		ss<<line;
		ss>>tag;

		if(tag == "ED"){
			OverlapInfo ov;
			ss>>sourceVertexId>>destinationVertexId>>ov.sourceOverlapStart>>ov.sourceOverlapEnd>>ov.sourceLength>>
			ov.destinationOverlapStart>>ov.destinationOverlapEnd>>ov.destinationLength>>ov.orientation>>ov.mismatch;

			checkEqualityInSga(edgeMapSga, sourceVertexId, destinationVertexId, ov);

			edgeMapSof[sourceVertexId][destinationVertexId]= ov;

		}
	}
	sgaFile.close();
	sofFile.close();
	std::printf("done collecting info\n");

	sgaFile = std::ifstream(sgaFileName);
	sofFile = std::ifstream(sofFileName);


	std::ofstream sgaOutFile(sgaFileName+".eq");
	std::ofstream sofOutFile(sofFileName+".eq");

	writeFileWithInfo(sgaFile,edgeMapSga,sgaOutFile);
	writeFileWithInfo(sofFile,edgeMapSof,sofOutFile);

	std::printf("finished\n");

}

