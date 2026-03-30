#include<bits/stdc++.h>
using namespace std;

double distance(pair<double,double> a, pair<double,double> b){
    return sqrt((a.first-b.first)*(a.first-b.first)+(a.second-b.second)*(a.second-b.second));
}
void policz_dla_pliku(const string& nazwa_pliku, ofstream& wyniki){
    ifstream plik(nazwa_pliku);
    vector<double> distances;
    int sum10=0,sum50=0;

    if(!plik.is_open()){
        cout<<"blad przy otwieraniu";
        return;
    }
    string linia;
    while (getline(plik, linia)) {
        if (!linia.empty() && linia.back() == '\r') linia.pop_back();
        if (linia == "NODE_COORD_SECTION") break;
    }
    vector<pair<double,double>> coordinates;
    while (getline(plik, linia)) {
        if (!linia.empty() && linia.back() == '\r') linia.pop_back();
        if (linia == "EOF") break;

        stringstream ss(linia);
        int id;
        double x, y;
        ss>>id>>x>>y;
        coordinates.push_back({x, y});
    }
    for(int i=1; i<=1000; i++){
        vector<pair<double,double>> permutation=coordinates;
        random_shuffle(permutation.begin(),permutation.end());
        double total_distance=0;
        for(int j=0; j<permutation.size(); j++){
            total_distance+=distance(permutation[j],permutation[(j+1)%permutation.size()]);
        }
        distances.push_back(total_distance);
        if(i%10==0){
            sum10+=*min_element(distances.end()-10,distances.end());
        }
        if(i%50==0){
            sum50+=*min_element(distances.end()-50,distances.end());
        }
    }
    int MIN=*min_element(distances.begin(),distances.end());
    int average10=sum10/100;
    int average50=sum50/20;
    wyniki << nazwa_pliku << " "  << average10 << " " << average50 << " " << MIN << endl;
}
int main(){
    vector<string> pliki = {"dj38.tsp", "qa194.tsp", "uy734.tsp", "wi29.tsp", "zi929.tsp"};
    ofstream wyniki("wyniki");
    for(const string& nazwa_pliku : pliki){
        policz_dla_pliku(nazwa_pliku, wyniki);
    }
    return 0;
}