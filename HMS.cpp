#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Bed {
    int bId;
    int pBed;
    bool reserved;
    int bedCharges;
};

struct Medicine {
    int mID;
    string name;
    string type;
    int stock;
    float pricePerItem;

};

struct Medication {
    int medicineId;
    string name;
    string type;
    int quantity;
    float price;

};

struct PatientInfo {
    int pID;
    string name;
    string disease;
    string dateAdmit;
    int cnic;

};

class BedManagement {
private:
    vector<Bed> beds;

public:
    BedManagement(int initialBedCount = 10, int defaultCharge = 1000) {
        beds.reserve(initialBedCount);
        for (int i = 0; i < initialBedCount; ++i) {
            Bed bed;
            bed.bId = i;
            bed.pBed = 0;
            bed.reserved = false;
            bed.bedCharges = defaultCharge;
            beds.push_back(bed);
        }
        cout<<"BedManagement Constructor Called"<<endl;
    }

    bool assignBed(int patientId) {
        for (int i = 0; i < (int)beds.size(); i++) {
            Bed &bed = beds[i];
            if (!bed.reserved) {
                bed.reserved = true;
                bed.pBed = patientId;
                cout << "Bed " << bed.bId << " assigned to patient #" << patientId << endl;
                return true;
            }
        }
        cout << "No bed available." << endl;
        return false;
    }

    void resignBed(int patientId) {
        for (int i = 0; i < (int)beds.size(); i++) {
            Bed &bed = beds[i];
            if (bed.reserved && bed.pBed == patientId) {
                bed.reserved = false;
                bed.pBed = 0;
                cout << "Bed " << bed.bId << " released." << endl;
            }
        }
    }

    int currentBedCharges(){
        if (beds.empty()) {
            return 0;
        }
        return beds.front().bedCharges;
    }

    void set() {
        int choice = 0;
        cout << "Enter 1 to add beds or\n2 to update bed charges: ";
        cin >> choice;

        if (choice == 1) {
            int toAdd = 0;
            cout << "Enter number of beds to add: ";
            cin >> toAdd;
            if (toAdd <= 0) {
                cout << "Invalid number." << endl;
                return;
            }
            int start = (int)beds.size();
            for (int i = 0; i < toAdd; ++i) {
                beds.push_back({start + i, 0, false, currentBedCharges()});
            }
            cout << "Total beds: " << beds.size() << endl;
        } 
		else if (choice == 2) {
            int newCharge = 0;
            cout << "Enter new bed charge: ";
            cin >> newCharge;
            if (newCharge < 0) {
                cout << "Bed charge cannot be negative." << endl;
                return;
            }
            for (int i = 0; i < (int)beds.size(); i++) {
                beds[i].bedCharges = newCharge;
            }
            cout << "Updated bed charges to " << newCharge << endl;
        } 
		else {
            cout << "Invalid choice." << endl;
        }
    }

    void get(){
        int choice = 0;
        cout << "Enter 0 for free beds:\n1 for reserved beds:\n2 for total beds: ";
        cin >> choice;
        switch (choice) {
            case 0:
                for (int i = 0; i < (int)beds.size(); i++) {
                    Bed &bed = beds[i];
                    if (!bed.reserved){
						cout << "Bed " << bed.bId << " is available." << endl;
					}
                }
                break;
            case 1:
                for (int i = 0; i < (int)beds.size(); i++) {
                    const Bed &bed = beds[i];
                    if (bed.reserved){
						cout << "Bed " << bed.bId << " is reserved by patient #" << bed.pBed << "." << endl;
					}
                }
                break;
            case 2:
                cout << "Total beds: " << beds.size() << endl;
                break;
            default:
                cout << "Invalid choice." << endl;
        }
    }

    void removeBed() {
        int id = 0;
        cout << "Enter bed ID to remove: ";
        cin >> id;
        if (id < 0 || id >= (int)beds.size()) {
            cout << "Invalid bed ID." << endl;
            return;
        }
        if (beds[id].reserved) {
            cout << "Cannot remove reserved bed." << endl;
            return;
        }
        beds.erase(beds.begin() + id);
        for (int i = 0; i < (int)beds.size(); ++i) {
        	beds[i].bId = i;
		}
        cout << "Bed removed." << endl;
    }

    void bedMenu() {
        int choice = 0;
        while (choice != 4) {
            cout << endl;
            cout << "[Bed Management]" << endl;
            cout << "1. Add/Update beds" << endl;
            cout << "2. View beds" << endl;
            cout << "3. Remove bed" << endl;
            cout << "4. Back" << endl;
            cout << "Choice: ";
            cin >> choice;
            switch (choice) {
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
					cout << "Invalid choice." << endl;
            }
        }
    }
};

class MedManagement {
private:
    vector<Medicine> medics;

public:
    MedManagement(int reserveCount = 0) {
        if (reserveCount > 0) {
            medics.reserve(reserveCount);
        }
        cout<<"MedManagement Constructor Called"<<endl;
    }

    void set() {
        char more = 'y';
        while (more == 'y' || more == 'Y') {
            Medicine m;
            m.mID = (int)medics.size();
            m.name = "";
            m.type = "";
            m.stock = 0;
            m.pricePerItem = 0.0;
            cout << "Enter medicine type: ";
            cin >> m.type;
            cout << "Enter medicine name: ";
            cin >> m.name;
            cout << "Enter price per item: ";
            cin >> m.pricePerItem;
            if (m.pricePerItem < 0){
                cout << "Price cannot be negative." << endl;
                continue;
            }
            cout << "Enter stock: ";
            cin >> m.stock;
            if (m.stock < 0) {
                cout << "Stock cannot be negative." << endl;
                continue;
            }
            medics.push_back(m);

            cout << "Add more medicines? (y/n): ";
            cin >> more;
        }
    }

    void get(){
        if (medics.empty()) {
            cout << "No medicines available." << endl;
            return;
        }
        for (int i = 0; i < (int)medics.size(); i++) {
            Medicine &m = medics[i];
            cout << "ID: " << m.mID << ", Name: " << m.name << ", Type: " << m.type
                 << ", Stock: " << m.stock << ", Price: " << m.pricePerItem << endl;
        }
    }

    void removeItem() {
        int id = 0;
        cout << "Enter medicine ID to remove: ";
        cin >> id;
        if (id < 0 || id >= (int)medics.size()) {
            cout << "Invalid ID." << endl;
            return;
        }
        medics.erase(medics.begin() + id);
        for (int i = 0; i < (int)medics.size(); ++i){
			medics[i].mID = i;
		}
        cout << "Medicine removed." << endl;
    }

    void updateStock() {
        int id = 0, add = 0;
        cout << "Enter medicine ID: ";
        cin >> id;
        if (id < 0 || id >= (int)medics.size()) {
            cout << "Invalid ID." << endl;
            return;
        }
        cout << "Enter stock to add: ";
        cin >> add;
        if (add < 0) {
            cout << "Stock increment cannot be negative." << endl;
            return;
        }
        medics[id].stock += add;
        cout << "New stock for " << medics[id].name << ": " << medics[id].stock << endl;
    }

    bool consumeMedicine(int id, int qty, Medication out){
        if (id < 0 || id >= (int)medics.size()){
			return false;
		}
        if (qty <= 0 || medics[id].stock < qty){
			return false;
		}

        medics[id].stock -= qty;
        out.medicineId = medics[id].mID;
        out.name = medics[id].name;
        out.type = medics[id].type;
        out.quantity = qty;
        out.price = medics[id].pricePerItem;
        return true;
    }

    void medicineMenu() {
        int choice = 0;
        while (choice != 5) {
            cout << endl;
            cout << "[Medicine Management]" << endl;
            cout << "1. Add medicines" << endl;
            cout << "2. Display medicines" << endl;
            cout << "3. Remove medicine" << endl;
            cout << "4. Update stock" << endl;
            cout << "5. Back" << endl;
            cout << "Choice: ";
            cin >> choice;
            switch (choice) {
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
					cout << "Invalid choice." << endl;
            }
        }
    }
};

class Patient {
private:
    static int nextId;
    PatientInfo info;
    vector<Medication> meds;

public:
    Patient(int presetCnic = 0,string presetName = "",string presetDisease = "",string presetDate = "") {
        info.pID = ++nextId;
        info.cnic = presetCnic;
        info.name = presetName;
        info.disease = presetDisease;
        info.dateAdmit = presetDate;
        cout<<"Patient Constructor Called"<<endl;
    }

    int id(){
		return info.pID; 
	}
    int cnic(){
		return info.cnic; 
	}
    void setCnic(int value){ 
		info.cnic = value; 
	}

    void set() {
        cout << "Enter patient name: ";
        cin >> info.name;
        if (info.cnic == 0) {
            cout << "Enter patient CNIC: ";
            cin >> info.cnic;
        }
        cout << "Enter disease: ";
        cin >> info.disease;
        cout << "Enter admit date (yyyy/mm/dd): ";
        cin >> info.dateAdmit;
        cout << "Assigned patient ID: " << info.pID << endl;
    }

    void patientHistory(){
        cout << "Patient ID: " << info.pID << endl;
        cout << "Name: " << info.name << endl;
        cout << "CNIC: " << info.cnic << endl;
        cout << "Disease: " << info.disease << endl;
        cout << "Admit Date: " << info.dateAdmit << endl;
    }

    void addMedication(MedManagement medManager) {
        int medId = 0, qty = 0;
        cout << "Enter medicine ID: ";
        cin >> medId;
        cout << "Enter quantity: ";
        cin >> qty;

        Medication med;
        if (medManager.consumeMedicine(medId, qty, med)) {
            meds.push_back(med);
            cout << "Medication added to patient." << endl;
        } else {
            cout << "Unable to add medication (invalid ID or insufficient stock)." << endl;
        }
    }

    void displayMedication(){
        if (meds.empty()) {
            cout << "No medications given." << endl;
            return;
        }
        for (int i = 0; i < (int)meds.size(); i++) {
            const Medication &m = meds[i];
            cout << "- " << m.name << " (" << m.type << "), qty=" << m.quantity
                 << ", unit price=" << m.price << endl;
        }
    }

    void generateBill(int bedChargePerDay){
        int days = 0;
        cout << "Enter number of days stayed: ";
        cin >> days;
        if (days < 0) {
            cout << "Days cannot be negative." << endl;
            return;
        }

        float medCharges = 0.0;
        for (int i = 0; i < (int)meds.size(); i++) {
            medCharges += (meds[i].price) * (meds[i].quantity);
        }
        float bedCharges = (float)(bedChargePerDay * days);
        float total = medCharges + bedCharges;

        cout << endl;
        cout << "Bill Summary" << endl;
        cout << "Medication Charges: " << medCharges << endl;
        cout << "Bed Charges: " << bedCharges << endl;
        cout << "Total: " << total << endl;
    }
};

int Patient::nextId = 0;

class PatientManagement {
private:
    vector<Patient> patients;
    BedManagement bedManager;
    MedManagement medManager;

    int findIndexByCnic(int cnic){
        for (int i = 0; i < (int)patients.size(); ++i) {
            if (patients[i].cnic() == cnic){
				return i;
			}
        }
        return -1;
    }

public:
    PatientManagement(BedManagement b, MedManagement m):bedManager(b), medManager(m){
    	cout<<"PatientManagement Constructor Called"<<endl;
	}

    void addPatient() {
        int cnic = 0;
        cout << "Enter patient CNIC: ";
        cin >> cnic;
        if (findIndexByCnic(cnic) != -1) {
            cout << "Patient with this CNIC already exists." << endl;
            return;
        }

        Patient p;
        p.setCnic(cnic);
        p.set();
        if (!bedManager.assignBed(p.id())) {
            cout << "Patient not added because no bed is available." << endl;
            return;
        }
        patients.push_back(p);
        cout << "Patient added successfully." << endl;
    }

    void removePatient() {
        int cnic = 0;
        cout << "Enter patient CNIC to remove: ";
        cin >> cnic;
        int idx = findIndexByCnic(cnic);
        if (idx == -1) {
            cout << "Patient not found." << endl;
            return;
        }

        bedManager.resignBed(patients[idx].id());
        patients.erase(patients.begin() + idx);
        cout << "Patient removed successfully." << endl;
    }

    void managePatient() {
        int cnic = 0;
        cout << "Enter patient CNIC: ";
        cin >> cnic;
        int idx = findIndexByCnic(cnic);
        if (idx == -1) {
            cout << "Patient not found." << endl;
            return;
        }

        int choice = 0;
        while (choice != 5) {
            cout << endl;
            cout << "[Patient Menu]" << endl;
            cout << "1. Show history" << endl;
            cout << "2. Add medication" << endl;
            cout << "3. Display medications" << endl;
            cout << "4. Generate bill" << endl;
            cout << "5. Back" << endl;
            cout << "Choice: ";
            cin >> choice;
            switch (choice) {
                case 1: 
					patients[idx].patientHistory(); 
					break;
                case 2: 
					patients[idx].addMedication(medManager); 
					break;
                case 3: 
					patients[idx].displayMedication(); 
					break;
                case 4: 
					patients[idx].generateBill(bedManager.currentBedCharges()); 
					break;
                case 5: 
					break;
                default: cout << "Invalid choice." << endl;
            }
        }
    }

    void patientManagementMenu() {
        int choice = 0;
        while (choice != 4) {
            cout << endl;
            cout << "[Patient Management]" << endl;
            cout << "1. Add patient" << endl;
            cout << "2. Remove patient" << endl;
            cout << "3. Manage patient" << endl;
            cout << "4. Back" << endl;
            cout << "Choice: ";
            cin >> choice;
            switch (choice) {
                case 1: addPatient(); break;
                case 2: removePatient(); break;
                case 3: managePatient(); break;
                case 4: break;
                default: cout << "Invalid choice." << endl;
            }
        }
    }
};

struct UserLogin {
    int uID;
    string pwd;
};

class HMS {
private:
    UserLogin user;
    BedManagement bedManager;
    MedManagement medManager;
    PatientManagement patientManager;

public:
    HMS(int defaultUserId = 4040,string defaultPassword = "Admin"):patientManager(bedManager, medManager) {
        user.uID = defaultUserId;
        user.pwd = defaultPassword;
        cout<<"HMS Constructor Called"<<endl;
    }
    
    bool Login(){
    	int tries = 3;
    	string uInputPwd = " ";
    	int uInputId = 0;
    	while(tries>0){
    		cout<<"Available Tries: "<<tries<<endl;
    		cout<<"Enter ID: ";
    		cin>>uInputId;
    		cout<<"Enter Password: ";
    		cin>>uInputPwd;
    		if(uInputId == user.uID && uInputPwd == user.pwd){
    			return true;
			}
			--tries;
		}
		return false;
	}

    void menu() {
        int choice = 0;
        if(!Login()){
        	cout<<endl;
        	cout<<"Too many Wrong Attempts"<<endl<<"Please try again after some time"<<endl;
        	return;
		}
        while (choice != 4) {
            cout << endl;
            cout << "Welcome to Hospital Management System" << endl;
            cout << "1. Patient Management" << endl;
            cout << "2. Medicine Management" << endl;
            cout << "3. Bed Management" << endl;
            cout << "4. Exit" << endl;
            cout << "Choice: ";
            cin >> choice;
            switch (choice) {
                case 1: 
					patientManager.patientManagementMenu(); 
					break;
                case 2: 
					medManager.medicineMenu(); 
					break;
                case 3: 
					bedManager.bedMenu(); 
					break;
                case 4: 
					cout << "Goodbye." << endl; 
					break;
                default: 
					cout << "Invalid choice." << endl;
            }
        }
    }
};

int main() {
    HMS obj;
    obj.menu();
    return 0;
}
