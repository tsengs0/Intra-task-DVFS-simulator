#include<iostream> //入出力ライブラリ
#include<fstream> //iostreamのファイル入出力をサポート
#include<string>
#include<sstream>
#include<vector>

using namespace std;

enum {
	col_1 = 0,
	col_2 = 1,
	col_3 = 2
};

#define FileFirstIndex 65
#define FileIndexDuration 5

int main(int argc, char **argv){
	string str;
	vector<float> rfj_acc, afj_acc, energy_acc;
	float rfj = 0, afj = 0, energy = 0;
	short state, file_index;
	file_index = (short) FileFirstIndex;
	char filename[100], output_FileName[100];
	
	if(argc != 3) {
		cout << "Please indicate the .CSV filename" << endl;
		exit(1);
	}
	

	sprintf(output_FileName,"Total_%s.%s.csv", argv[1], argv[2]);
	ofstream ofs(output_FileName); //ファイル出力ストリーム

while(file_index < 105) {
	sprintf(filename,"../../result/csv/Alpha_%d/%s.%s.csv", (int) file_index, argv[1], argv[2]);
	cout << "Reading " << filename << endl;
	//ファイルの読み込み
	ifstream ifs(filename);
 	if(!ifs){
		cout<<"入力エラー";
		exit(1);
	}

    //csvファイルを1行ずつ読み込む
    while(getline(ifs,str)) {
        string token;
        istringstream stream(str);
	state = (short) col_1;

        //1行のうち、文字列とコンマを分割する
        while(getline(stream,token,',')) {
            //すべて文字列として読み込まれるため
            //数値は変換が必要
            if(state == (short) col_1) {
		rfj_acc.push_back(stof(token));
		cout << stof(token) << ",";
	    }
            else if(state == (short) col_2) {
		afj_acc.push_back(stof(token));
		cout << stof(token) << ",";
	    }
            else {
		energy_acc.push_back(stof(token));
		cout << stof(token);
	    }
		state += 1;
        }
        cout<<endl;
    }
	if(rfj_acc.size() != afj_acc.size() || rfj_acc.size() != energy_acc.size() || afj_acc.size() != energy_acc.size()) {
		cout << "The number of contents among all column is not consistent" << endl;
		exit(1);
	}

	for(unsigned int i = 0; i < rfj_acc.size(); i++) {
		rfj += rfj_acc[i];
		afj += afj_acc[i];
		energy += energy_acc[i];

	}
	rfj = rfj / rfj_acc.size();
	afj = afj / afj_acc.size();
	energy = energy / energy_acc.size();

	ofs << rfj << ","
	    << afj << ","
	    << energy << endl;

	file_index += (short) FileIndexDuration;
	ifs.close();
}
    return 0;
}


