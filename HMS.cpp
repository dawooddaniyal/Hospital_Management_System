#include <iostream>
#include <vector>
using namespace std;

//Bed Management
struct Beds{
	int bId;
	bool status;
};
class bedManagement:patient{
	private:
		vector<Beds> beds[10];
		int count,i,choice;
	public:
		bedManagement(){
			for(i = 0; i<10; i++){
				beds[i].bId = i;
				beds[i].status = 0;
			}	
			i = 0;
			count = 10;
			choice = 0;
			cout<<"Bed Management Constructor"<<endl;
		}
		void set(){
			cout<<"Enter numbers of beds to add: ";
			cin>>choice;
			if(choice>0 && choice<=100){
				for(i=count+1;i<=choice;i++){
					beds.push_back({i,0});
					count++;
				}
				cout<<"Number of beds = "<<count<<endl;
			}
			else{
				cout<<"Invalid number. Try Again"<<endl;
			}
		}
		void get(){
			cout<<"Enter 0 to see free beds\n1 to see reserved beds: "<<endl;
			cout<<"Enter 2 to see total beds: "<<endl;
			cin>>choice;
			switch choice:
				case 0:
					for(i=0;i<count;i++){
						if(beds[i].status == 0){
							cout<<"Bed with id "<<beds[i].bId<<"is available."<<endl; 
						}
					}
					break;
				case 1:
					for(i=0;i<count;i++){
						if(beds[i].status == 0){
							cout<<"Bed with id "<<beds[i].bId<<"is Reserved."<<endl; 
						}
					}
					break;
				case 2:
					cout<<"Total beds are: "<<count<<endl;
					break;
				default:
					cout<<"Invalid Choice!"<<endl;
		}
		void removeBed{
			cout<<"Enter Bed Id to remove: ";
			cin>>choice;
			if(choice>=0 && choice<=count)
				if(beds[choice].status == 0){
					beds.remove(beds.begin() + choice);
					cout<<"Removed Bed with ID: "<<choice<<endl;
				}
				else{
					cout<<"Bed is reserved. First free the bed then try again. Thank You."<<endl;
				}
			}
		}
		~bedManagement(){
			cout<<"Bed Management Destructor"<<endl;
		}
};

//Medicine Management
struct Medics{
	int stock;
	string name,type;
	float pricePerItem;
};
class medManagement{
	private:
		vector<Medics> medics[10];
		nt;
	public:
		
};

//Patient Management
struct Medication{
	int quantity;
	string name,type;
	float price;
};
struct pInfo{
	string name,disease;
	int CNIC,phNo,dateAdmit,pID;
	Medication med;
};
patient::newID = 0;
class patient{
	private:
		pInfo pData;
		static int newID;
		bill pBill;
	public:
		patient(){
			newID++;
			pData.pID = newID;
		}
};
class patientManagement:public bedManagement{
	private:
		vector<patient> patients;
	public: 
		void assignBed(){
			
			for(i=0;i<count;i++){
				if(beds[i].status == 0){
				cout<<"Bed with id "<<beds[i].bId<<"is available."<<endl; 
			}
			cout<<"Enter bed # to assign"<<endl;
			cin>>choice;
		
			if(choice>=0 && choice <=count){
				if(beds[choice].status == 0){
					cout<<"Bed Assigned to patient with ID # "<<pData.pID<<endl;
				}
				else{
					cout<<"Sorry, Bed is not available.\n Please choose from available beds."<<endl;
				}
			}
			else{
				cout<<"Invalid Choice."<<endl;
			}
		}
};
class bill{
	private:
		float totalBill;
	public:
};

//Controller
class HMS{
	private:
	patientManagement pManage;
	bedManagement bManage;
	medManagement mManage;
	public:
		
};

//Main Function
int main(){
	
	return 0;
}