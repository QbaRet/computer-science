#include<iostream>
#include<vector>
using namespace std;
bool search(vector<vector<int>>&matrix, int target)
{
    int m=matrix.size();
    int n=matrix[0].size();
    int i=0;
    int j=n-1;
    while(i<m && j>=0)
    {
        if(matrix[i][j]==target)
            return true;
        else if(matrix[i][j]>target)
            j--;
        else
            i++;
    }
    return false;
}
int main()
{
    int n,m;
    cout<<"Podaj rozmiar n i m: "<<endl;
    cin>>n>>m;
    int target;
    cout<<"Podaj szukana liczbe: "<<endl;
    cin>>target;
    vector<vector<int>> matrix(m,vector<int>(n));
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
            cout<<"Podaj element ["<<i<<"]["<<j<<"]: "<<endl;
            cin>>matrix[i][j];
        }
    }
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
            cout<<matrix[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"Czy liczba "<<target<<" znajduje sie w macierzy: "<<(search(matrix,target)?"Tak":"Nie")<<endl;
}