#include <iostream>
#include <vector>
using namespace std;

//Bed Management
struct Beds{
	int bId,pBed;
	bool status;
	int bedCharges;
};

class bedManagement{
	private:
		vector<Beds> beds[10];
		int count,i,choice;
		friend class patient::generateBill;
		friend class patientManagement::assignBed(),resignBed();
	public:
		bedManagement(){
			for(i = 0; i<10; i++){
				beds[i].bId = i;
				beds[i].status = 0;
				beds[i].bedCharges = 1000;
			}	
			i = 0;
			pBed = 0;
			count = 10;
			choice = 0;
			cout<<"Bed Management Constructor"<<endl;
		}
		void set(){
			cout<<"Enter 1 to add beds or 2 to update bed charges: ";
			cin>>choice;
			if(choice == 1){
			
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
			else if(choice == 2){
				cout<<"Enter new price of bed charges: ";
				cin>>choice;
				for(i=0;i<count;i++){
					beds[i].bedCharges = choice;
				}
				cout<<"Updated bed charges to "<<choice<<endl;
			}
			else{
				cout<<"Invalid Choice"<<endl;
			}
		}
		void get(){
			cout<<"Enter 0 to see free beds\n1 to see reserved beds: "<<endl;
			cout<<"Enter 2 to see total beds: "<<endl;
			cin>>choice;
			switch choice{
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
		void bedMenu(){
			choice = 0;
			while(choice!=4){
				cout<<"Enter 1 to add beds: "<<endl;
				cout<<"Enter 2 to see beds: "<<endl;
				cout<<"Enter 3 to remove bed:"<<endl;
				cout<<"Enter 4 to Exit: "<<endl;
				cin>>choice;
				switch(choice){
					case 1:
						set();
						break;
					case 2: 
						get();
						break;
					case 3:
						removeBed();
						break;
					case 4:
						break;
					default:
						cout<<"Invalid Choice"<<endl;
				}
			}
		}
		~bedManagement(){
			cout<<"bedManagement Destructor Called."<<endl;
		}
};

//Medicine Management
struct Medics{
	int stock,mID;
	string name,type;
	float pricePerItem;
};
class medManagement{
	private:
		vector<Medics> medics[10];
		int i,count,choice;
		friend class patient::addMedication();
	public:
		medManagement(){
			for(i=0;i<10;i++){
				medics[i].stock = 0;
				medics[i].name = " ";
				medics[i].type = " ";
				medics[i].pricePerItem = 0;
				medics[i].mID = i;
			}
			choice = 0;
			count = 0;
			i = 0;
		};
		void set(){
			while(choice == 0){
			cout<<"Enter 0 to set medical items or 1 to exit: ";
			cin>>choice;
			if(choice == 1){
				cout<<"Enter type of item(Tablets,Injections,Syrups,etc): ";
				cin>>medics[count].type;
				cout<<"Enter name of item: ";
				cin>>medics[count].name;
				cout<<"Enter price per item: ";
				cin>>medics[count].pricePerItem;
				cout<<"Enter stock to add: ";
				cin>>medics[count].stock;
				medics[count].mID = count;
				count++;
			}
			}
		}
		void get(){
			for(i=0;i<count;i++){
				cout<<"Item ID: "<<medics[i].mID<<endl;
				cout<<"Item name: "<<medics[i].name<<endl;
				cout<<"Item type: "<<medics[i].type<<endl;
				cout<<"Item Stock: "<<medics[i].stock<<endl;
				cout<<"Per Item Price: "<<medics[i].price<<endl;
				cout<<endl;
			}
			cout<<"Total Items = "<<count<<endl;
		}
		void removeItem(){
			cout<<"Enter Id to remove item: ";
			cin>>choice;
			if(choice>=0 && choice<count){
				medics.erase(medics.begin(),choice);
				cout<<"Removed Item: "<<endl;
				cout<<"\tItem ID: "<<medics[choice].mID<<endl;
				cout<<"\tItem name: "<<medics[choice].name<<endl;
				cout<<"\tItem type: "<<medics[choice].type<<endl;
				cout<<endl;
				count--;
				cout<<"Total Items = "<<count<<endl;
			}
			else{
				cout<<"No item with ID # "<<choice<<" found!"<<endl;
			}
		}
		void updateStock(){
			int choice;
			cout<<"Enter Id to update Stock: ";
			cin>>choice;
			if(choice>=0 && choice<count){
				cout<<"\tItem ID: "<<medics[choice].mID<<endl;
				cout<<"\tItem name: "<<medics[choice].name<<endl;
				cout<<"\tItem type: "<<medics[choice].type<<endl;
				cout<<"Enter stock to add: "<<endl;
				cin>>choice;
				medics[this->choice].stock += choice;
				cout<<"Update Stock of following Item to : "<<medics[this->choice].stock<<endl;
			}
			else{
				cout<<"Invalid Id!"<<endl;
			}
		}
		void medicineMenu(){
			choice = 0;
			while(choice != 5){
				cout<<"Enter 1 to add medical items: "<<endl;
				cout<<"Enter 2 to display medical items: "<<endl;
				cout<<"Enter 3 to remove item: "<<endl;
				cout<<"Enter 4 to update stock: "<<endl;
				cout<<"Enter 5 to Exit: "<<endl;
				cin>>choice;
				switch(choice){
					case 1:
						set();
						break;
					case 2:
						get();
						break;
					case 3:
						removeItem();
						break;
					case 4:
						updateStock();
						break;
					case 5:
						break;
					default:
						cout<<"Invalid Choice"<<endl;
						break;
				}
			}
		}
		~medManagement(){
			cout<<"medManagement Destructor Called."<<endl;
		}
};

//Patient Management
struct Medication{
	int quantity;
	string name,type;
	float price;
};
struct pInfo{
	string name,disease,dateAdmit;
	int CNIC,pID;
};
patient::newID = 0;
class patient{
	private:
		vector<Medication> med[1];
		int mCount,bCount,i,choice;
		pInfo pData;
		float totalBill,totalBedCharges,totalMedCharges;
		static int newID;
		vector<float> medCharges[1];
		int bCount;
		friend patientManagement::search();
	public:
		patient(){
			newID++;
			pData.pID = newID;
			mCount,bCount = 0;
			pData.name,pData.disease,pData.dateAdmit = " ";
			pData.CNIC = 0;
			totalBill,totalBedCharges,totalMedCharges = 0;
			med[0].name,med[1].price,med[2].quantity,med[3].type = " ";
			medCharges[1] = 0;
			cout<<"Patient Constructor Called"<<endl;
		}
		void set(){
			cout<<"Enter patient Name";
			cin>>pData.name;
			cout<<"Enter patient CNIC";
			cin>>pData.CNIC;
			cout<<"Enter Disease: ";
			cin>>pData.disease;
			cout<<"Enter Date Admitted (yyyy/mm/dd): ";
			cin>>pData.dateAdmit;
			pData.dateAdmit.insert(4,"/");
			pData.dateAdmit.insert(7,"/");
			cout<<"Patient has beed assigned ID: "<<pData.pID<<endl;
		}
		void get(){
			cout<<"Patient ID: "<<pData.pID<<endl;
			cout<<"Patient Name: "<<pData.name<<endl;
			cout<<"Patient CNIC: "<<pData.CNIC<<endl;
			cout<<"Disease: "<<pData.disease<<endl;
			cout<<"Date Admitted (yyyy/mm/dd): "<<pData.dateAdmit<<endl;
		}
		void patientHistory(){
			cout<<"Patient Name: "<<pData.name<<endl;
			cout<<"Patient was admitted on: "<<pData.dateAdmit<<endl;
			cout<<"Patient has Disease: "<<pData.disease<<endl;
		}
		void addMedication(){
			cout<<"Enter ID of medication: ";
			cin>>choice;
			cout<<"Enter quantity: ";
			cin>>med.quantity;
			if(choice>=0 && choice<count){
				med[mCount].name = medics[choice].name;
				med[mCount].type = medics[choice].type;
				medics[choice].stock -=med[mCount].quantity;
				med[mCount].price = medics[choice].pricePerItem;
				mCount++;
				medCharges[bCount]. = (med[mCount].price)*(med[mCount].quantity);
				bCount++;
				cout<<"Medication has beed added."<<endl;
			}
			else if(choice<0 || choice>=count){
				cout<<"Invalid Choice"<<endl;
			}
			else{
				cout<<"Sorry.Ran out of stock."<<endl;
			}
			}
		}
		void displayMedication(){
			if(mCount>0){
				for(i=0;i<mCount;i++){
				cout<<"\t\tMedication # "<<i<<endl;
				cout<<"\t\tMedication Name: "<<med[i].name<<endl;
				cout<<"\t\tMedication Type: "<<med[i].type<<endl;
				cout<<"\t\tMedication Quantity; "<<med[i].quantity<<endl;
				cout<<endl;
				}
			}
			else{
				cout<<"\t\tNo medications given."<<endl;
			}
		}
		void generateBill(){
			for(i=0;i<bCount;i++){
				totalMedCharges+=medCharges[i];
			}
			
			cout<<"Enter number of days stayed: ";
			cin>>choice;
			totalBedCharges = bedCharges*choice;
			totalBill += totalBedCharges + totalMedCharges;
			
			cout<<"Bill:  "<<endl;
			cout<<"\tMedical Charges: "<<endl;
			displayMedication();
			cout<<"\tTotal Medical Charges = Rs."<<totalMedCharges<<endl;
			cout<<"\n\tBed Charges: "<<totalBedCharges<<endl;
			cout<<"\n\nYour total bill is: Rs."<<totalBill<<endl;
		}
		void patientMenu(){
			choice = 0;
			while(choice != 5){
				cout<<"Enter 1 to See Patient History: "<<endl;
				cout<<"Enter 2 to Add Medication: "<<endl;
				cout<<"Enter 3 to Display Medication: "<<endl;
				cout<<"Enter 4 to Generate Bill: "<<endl;
				cout<<"Enter 5 to exit: "<<endl;
				cin>>choice;
				switch(choice){
					case 1:
						patientHistory();
						break;
					case 2:
						addMedication();
						break;
					case 3:
						displayMedication();
						break;
					case 4:
						generateBill();
						break;
					case 5:
						break;
					default:
						cout<<"Invalid Choice"<<endl;
				}
			}
		}
		~patient(){
			cout<<"Patient Object Destroyed"<<endl;
		}
};

class patientManagement{
	private:
		vector<patient> patients[1];
		int i,choice,pMCount;
		bool found;
	public: 
		patientManagement(){
			patients[1]
			pMCount = 0;
			i = 0;
			choice = 0;
			found = false;
			cout<<"Patient Management Constructor"<<endl;
		}
		void set(){
			search();
			if(!found){
				cout<<"No Patient Found."<<endl;
				cout<<"Entering new Patient Info."<<endl;
				patients[pMCount].set();
				assignBed();
				assignBed(pMCount);
				pMCount++;
				found = false;
			}
			else{
				cout<<"Patient Already Exists in Data."<<endl;
				found = false;
			}
		}
		void get(){
			search();
			if(found){
				cout<<"Displaying Patient Info."<<endl;
				patients[count].get();
				found = false;
			}
			else{
				cout<<"No Patient found."<<endl;
				found = false;
			}
		}
		void search(){
			cout<<"Enter patient CNIC: "<<endl;
			cin>>choice;
			for(i=0;i<pMCount;i++){
				if(choice == patient[i].pData.CNIC){
					found = true;
					break;
				}
			}
			if(found){
				cout<<"Patient Found."<<endl;
			}
		}
		void removePatient(){
			search();
			if(found){
				delete patients[i];
				resignBed();
				cout<<"Removed Patient Successfully"<<endl;
				pMCount--;
				found = false;
			}
			else{
				cout<<"Sorry. No patient found."<<endl;
				found = false;
			}
		}
		void patientManage(){
			search();
			if(found){
				patients[i].patientMenu();
				found = false;
			}
			else{
				cout<<"Patient Not Found"<<endl;
				found = false;
			}
		}
		void patientManagementMenu(){
			choice = 0;
			while(choice != 4){
				cout<<"Welcome to Patient Management System."<<endl<<endl;
				cout<<"Enter 1 to Add Patient: "<<endl;
				cout<<"Enter 2 to Remove Patient: "<<endl;
				cout<<"Enter 3 to Manage Specific Patient: "<<endl;
				cout<<"Enter 4 to Exit."<<endl;
				cin>>choice;
				switch(choice){
				
					case 1:
						set();
						break;
					case 2:
						removePatient();
						break;
					case 3:
						patientManage();
						break;
					case 4:
						break;
					default:
						cout<<"Invalid Choice"<<endl;
						
				}
			}
		}
		
		void assignBed(int newPId){
					
			for(i=0;i<count;i++){
				if(beds[i].status == 0){
					cout<<"Bed Assigned to patient with ID # "<<newPId<<endl;
					beds[i].pBed = newPId;
					beds[i].status = 1;
					break;				
				}
			}
			if(i==count){
				cout<<"Sorry, Bed is not available.\n Please choose from available beds."<<endl;
			}
		}
		void resignBed(int newPId){
			for(i=0;i<count;i++){
				if(beds[i].pBed == newPId)
					cout<<"Bed with Id # "<<beds[i].bId<<" has beed resigned."<<endl;
					beds[i].status = 0;
					beds[i].pBed = 0;
					break;
			}
		}
		~bedManagement(){
			cout<<"bedManagement Destructor"<<endl;
		}
};

//Controller
struct userLogin{
	int uID;
	string pwd;
};
class HMS{
	private:
	int choice,i,count;
	vector<userLogin> loginInfo[1];
	patientManagement pManage;
	bedManagement bManage;
	medManagement mManage;
	public:
		HMS(){
			choice = 0;
			count = 1;
			loginInfo[1].uID = 4040;
			loginInfo[1].pwd = "Admin";
			pManage = new patientManagement;
			bManage = new bedManagement;
			mManage = new medManagement;
			cout<<"HMS Constructor"<<endl;
		}
		void set(){
			cout<<"Adding new user: "<<endl;
			count++;
			if(count<10){
				cout<<"Enter new Id: ";
				cin>>loginInfo[count].uID;
				cout<<"Enter new password: ";
				cin>>loginInfo[count].pwd;
			}
			else{
				cout<<"Limit Reached.\nCannot add more users."<<endl;
			}
		}
		void get(){
			cout<<"Following are users and passwords: ";
			for(i=0;i<count;i++){
				cout<<"\n\nUser Id: "<<loginInfo[i].uID<<" , Pssword: "<<loginInfo[i].pwd<<endl;
			}	
		}
		void menu(){
			cout<<"Welcome to Hospital Management System."<<endl
			choice = 0;
			while(choice != 4){
				cout<<"Enter 1 to open Patient Management: "<<endl;
				cout<<"Enter 2 to open Medicine Management: "<<endl;
				cout<<"Enter 3 to open Bed Management: "<<endl;
				cout<<"Enter 4 to Exit: "<<endl;
				cin>>choice;
				switch(choice){
					case 1:
						pManage.patientManagementMenu();
						break;
					case 2:
						mManage.medicineMenu();
						break;
					case 3:
						bManage.bedMenu();
						break;
					case 4:
						break;
					default:
						cout<<"Invalid Choice!"<<endl;
				}
			}
		}
		~HMS(){
			cout<<"HMS Destructor"<<endl;
		}
};

//Main Function
int main(){
	HMS obj;
	obj.menu();
	return 0;
}
