#include<bits/stdc++.h>
using namespace std;
int main(){
	double n;
	int m = sqrt(n);
	if(n == m*m){
		cout<<"N\n";
	}
	else {
		int ok = 0;
		for(int i = 2; i <= sqrt(m) ; i++){
			if(m%i == 0){
				ok = 1;
				break;
			}
		}
		if(ok==1)
			cout<< "S\n";
		else cout << "N\n";
	}


}