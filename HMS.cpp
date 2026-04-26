

#include <iostream>
#include <conio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstring>

using namespace std;

struct Bed {
    int    bId;
    int    pBed;
    bool   reserved;
    double bedCharges;   
};

struct Medicine {
    int    mID;
    string name;
    string type;
    int    stock;
    double pricePerItem; 
};

struct Medication {
    int    medicineId;
    string name;
    string type;
    int    quantity;
    double price;        
};

struct PatientInfo {
    int       pID;
    string    name;
    string    disease;
    
    long long cnic;     
};

class BedManagement {
private:
    vector<Bed> beds;
    void saveToFile(){
    	ofstream file("beds.txt");
    	if(!file.is_open()){
    		cout<<"Error: could not open from beds.txt for writing."<<endl;
    		return;
		}
		for(int i = 0;i < (int)beds.size();i++){
			file<<beds[i].bId<<"|"<<beds[i].pBed<<"|"<<beds[i].reserved<<"|"<<beds[i].bedCharges<<"\n";
		}
		file.close();
	}
public:
    BedManagement(int initialBedCount = 10, double defaultCharge = 1000.0) {
        beds.reserve(initialBedCount);
        for (int i = 0; i < initialBedCount; ++i) {
            Bed bed;
            bed.bId        = i;
            bed.pBed       = 0;
            bed.reserved   = false;
            bed.bedCharges = defaultCharge;
            beds.push_back(bed);
        }
        cout << "BedManagement Constructor Called" << endl;
    }
	
	void loadFromFile(){
		ifstream file("beds.txt");
		if(!file.is_open()){
			cout<<"No beds file found. Starting fresh"<<endl;
			return;
		}
		beds.clear();
		string line;
		while(getline(file,line)){
			if(line.empty()){
				continue;
			}
			stringstream ss(line);
			string token;
			Bed bed;
			getline(ss,token,'|');
			bed.bId = stoi(token);
			getline(ss,token,'|');
			bed.pBed = stoi(token);
			getline(ss,token,'|');
			bed.reserved = stoi(token);
			getline(ss,token,'|');
			bed.bedCharges = stod(token);
			beds.push_back(bed);
		}
		file.close();
		cout<<"Beds Loaded"<<beds.size()<<endl;
	}
    
    bool assignBed(int patientId, double &assignedCharge) {
        for (int i = 0; i < (int)beds.size(); i++) {
            Bed &bed = beds[i];
            if (!bed.reserved) {
                bed.reserved   = true;
                bed.pBed       = patientId;
                assignedCharge = bed.bedCharges;
                cout << "Bed " << bed.bId << " assigned to patient #" << patientId << endl;
                saveToFile();
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
                bed.pBed     = 0;
                cout << "Bed " << bed.bId << " released." << endl;
                saveToFile();
                return; 
            }
        }
    }

    double currentBedCharges() const {
        if (beds.empty()) return 0.0;
        return beds.front().bedCharges;
    }

    void set() {
        int choice = 0;
        cout << "\n[Bed Setup]\n"
                "Enter 1 to add new beds.\n"
                "Enter 2 to update per-day bed charges for all beds.\n"
                "Choice: ";
        cin >> choice;
        if (choice == 1) {
            int toAdd = 0;
            cout << "Enter number of beds to add: ";
            cin >> toAdd;
            if (toAdd <= 0) { cout << "Invalid number." << endl; return; }
            int start = (int)beds.size();
            for (int i = 0; i < toAdd; ++i)
                beds.push_back({start + i, 0, false, currentBedCharges()});
            cout << "Total beds: " << beds.size() << endl;
            saveToFile();
        } else if (choice == 2) {
            double newCharge = 0.0;
            cout << "Enter new bed charge: ";
            cin >> newCharge;
            if (newCharge < 0) { cout << "Bed charge cannot be negative." << endl; return; }
            for (int i = 0; i < (int)beds.size(); i++) beds[i].bedCharges = newCharge;
            cout << "Updated bed charges to " << newCharge << endl;
            saveToFile();
        } else {
            cout << "Invalid choice." << endl;
        }
    }

    void get() {
        int choice = 0;
        cout << "\n[Bed View]\n"
                "Enter 0 to list free beds.\n"
                "Enter 1 to list reserved beds.\n"
                "Enter 2 to show total bed count.\n"
                "Choice: ";
        cin >> choice;
        switch (choice) {
            case 0:
                for (int i = 0; i < (int)beds.size(); i++)
                    if (!beds[i].reserved)
                        cout << "Bed " << beds[i].bId << " is available." << endl;
                break;
            case 1:
                for (int i = 0; i < (int)beds.size(); i++)
                    if (beds[i].reserved)
                        cout << "Bed " << beds[i].bId << " reserved by patient #" << beds[i].pBed << endl;
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
        
        int foundIdx = -1;
        for (int i = 0; i < (int)beds.size(); ++i)
            if (beds[i].bId == id) { foundIdx = i; break; }
        if (foundIdx == -1) { cout << "Invalid bed ID." << endl; return; }
        if (beds[foundIdx].reserved) { cout << "Cannot remove reserved bed." << endl; return; }
        beds.erase(beds.begin() + foundIdx);
        for (int i = 0; i < (int)beds.size(); ++i) beds[i].bId = i;
        cout << "Bed removed." << endl;
        saveToFile();
    }

    void bedMenu() {
        int choice = 0;
        while (choice != 4) {
            cout << "\n[Bed Management]\n1. Add/Update beds\n2. View beds\n3. Remove bed\n4. Back\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: set();       break;
                case 2: get();       break;
                case 3: removeBed(); break;
                case 4:              break;
                default: cout << "Invalid choice." << endl;
            }
        }
    }
};

class MedManagement {
private:
    vector<Medicine> medics;
    
    int findIndexById(int mID) {
        for (int i = 0; i < (int)medics.size(); ++i)
            if (medics[i].mID == mID) return i;
        return -1;
    }
	void saveToFile(){
		ofstream file("medicines.txt");
		if(!file.is_open()){
			cout<<"Error: could not open medicines.txt for writing."<<endl;
			return;
		}
		for(int i = 0; i < (int)medics.size();i++){
			file<<medics[i].mID<<"|"<<medics[i].name<<"|"<<medics[i].type<<"|"<<medics[i].stock<<"|"<<medics[i].pricePerItem<<"\n";
		}
		file.close();
	}
public:
    MedManagement(int reserveCount = 0) {
        if (reserveCount > 0) medics.reserve(reserveCount);
        cout << "MedManagement Constructor Called" << endl;
    }
    
    void loadFromFile(){
    	ifstream file("medicines.txt");
    	if(!file.is_open()){
    		cout<<"No medicine file found. Starting fresh"<<endl;
    		return;
		}
		medics.clear();
		string line;
		while(getline(file,line)){
			if(line.empty()){
				continue;
			}
			stringstream ss(line);
			string token;
			Medicine m;
			getline(ss,token,'|');
			m.mID = stoi(token);
			getline(ss,token,'|');
			m.name = token;
			getline(ss,token,'|');
			m.type = token;
			getline(ss,token,'|');
			m.stock = stoi(token);
			getline(ss,token,'|');
			m.pricePerItem = stod(token);
			medics.push_back(m);
		}
		file.close();
		cout<<"Medicines Loaded: "<<medics.size()<<endl;
	}

    void set() {
        char more = 'y';
        cout << "\n[Medicine Entry]\nYou can add one or more medicines.\n";
        while (more == 'y' || more == 'Y') {
            Medicine m;
            m.mID          = (int)medics.size();
            m.name         = "";
            m.type         = "";
            m.stock        = 0;
            m.pricePerItem = 0.0;
            cout << "Enter medicine type: ";
            getline(cin >> ws, m.type);
            cout << "Enter medicine name: ";
            getline(cin >> ws, m.name);
            cout << "Enter price per item: ";
            cin >> m.pricePerItem;
            if (m.pricePerItem < 0) { cout << "Price cannot be negative." << endl; continue; }
            cout << "Enter stock: ";
            cin >> m.stock;
            if (m.stock < 0) { cout << "Stock cannot be negative." << endl; continue; }
            medics.push_back(m);
            saveToFile();
            cout << "Add more medicines? (y/n): ";
            cin >> more;
        }
    }

    void get() {
        if (medics.empty()) { cout << "No medicines available." << endl; return; }
        for (int i = 0; i < (int)medics.size(); i++) {
            const Medicine &m = medics[i];
            cout << "ID: " << m.mID << ", Name: " << m.name << ", Type: " << m.type
                 << ", Stock: " << m.stock << ", Price: " << m.pricePerItem << endl;
        }
    }

    void removeItem() {
        int id = 0;
        cout << "Enter medicine ID to remove: ";
        cin >> id;
        
        int idx = findIndexById(id);
        if (idx == -1) { cout << "Invalid ID." << endl; return; }
        medics.erase(medics.begin() + idx);
        for (int i = 0; i < (int)medics.size(); ++i) medics[i].mID = i;
        cout << "Medicine removed." << endl;
        saveToFile();
    }

    void updateStock() {
        int id = 0, add = 0;
        cout << "\n[Medicine Stock Update]\n";
        cout << "Enter medicine ID: ";
        cin >> id;
        
        int idx = findIndexById(id);
        if (idx == -1) { cout << "Invalid ID." << endl; return; }
        cout << "Enter stock to add: ";
        cin >> add;
        if (add < 0) { cout << "Stock increment cannot be negative." << endl; return; }
        medics[idx].stock += add;
        cout << "New stock for " << medics[idx].name << ": " << medics[idx].stock << endl;
        saveToFile();
    }

    
    bool consumeMedicine(int id, int qty, Medication &out) {
        int idx = findIndexById(id); 
        if (idx == -1) return false;
        if (qty <= 0 || medics[idx].stock < qty) return false;
        medics[idx].stock -= qty;
        saveToFile();
        out.medicineId = medics[idx].mID;
        out.name       = medics[idx].name;
        out.type       = medics[idx].type;
        out.quantity   = qty;
        out.price      = medics[idx].pricePerItem;
        return true;
    }

    void medicineMenu() {
        int choice = 0;
        while (choice != 5) {
            cout << "\n[Medicine Management]\n1. Add medicines\n2. Display medicines\n"
                    "3. Remove medicine\n4. Update stock\n5. Back\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: set();         break;
                case 2: get();         break;
                case 3: removeItem();  break;
                case 4: updateStock(); break;
                case 5:                break;
                default: cout << "Invalid choice." << endl;
            }
        }
    }
};

class Patient {
private:
    static int        nextId;
    PatientInfo       info;
    bool              isAdmitted;      
    bool              isDischarged;    
    time_t            admitTime;
    time_t            dischargeTime;
    struct tm         dateTime;
    double            admitBedCharge;  
    vector<Medication> meds;

public:
    Patient(long long presetCnic = 0, string presetName = "", string presetDisease = "") {
        info.pID       = 0;
        info.cnic      = presetCnic;
        info.name      = presetName;
        info.disease   = presetDisease;
        isAdmitted     = false;
        isDischarged   = false;
        admitTime      = 0;
        dischargeTime  = 0;
        admitBedCharge = 0.0;          
        memset(&dateTime, 0, sizeof(dateTime));
        cout << "Patient Constructor Called" << endl;
    }

    int       id() const              { return info.pID;     }
    long long getCnic() const         { return info.cnic;    } 
    bool      getIsDischarged() const { return isDischarged; }
	const PatientInfo& getInfo() const { return info; }
	bool getIsAdmitted() const { return isAdmitted; }
	time_t getAdmitTime() const { return admitTime;}
	time_t getDischargeTime() const { return dischargeTime;}
	double getAdmitBedCharges() const { return admitBedCharge;}
	const vector<Medication> &getMeds() const { return meds;}
	static int getNextId() { return nextId;}
	static void setNextId(int val){ nextId = val; }
	
	void setName(const string &val){info.name = val;}
	void setDisease(const string &val){info.disease = val;}
	void restoreState(int pid, bool admitted, bool discharged, time_t aTime, time_t dTime, double charge){
		info.pID = pid;
		isAdmitted = admitted;
		isDischarged = discharged;
		admitTime = aTime;
		dischargeTime = dTime;
		admitBedCharge = charge;
		if(admitTime != 0){
			dateTime = *localtime(&admitTime);
			mktime(&dateTime);
		}
	}
	void addMedicationDirect(const Medication &med){
		meds.push_back(med);
	}
    void setCnic(long long value) { info.cnic = value; }

    
    void markAdmitted(double bedCharge) {
        if (!isAdmitted) {
            admitTime      = time(nullptr);
            dateTime       = *localtime(&admitTime);
            mktime(&dateTime);
            isAdmitted     = true;
            admitBedCharge = bedCharge; 
            info.pID       = ++nextId;  
            cout << "Patient admitted. ID: " << info.pID << endl;
        }
    }

    void markDischarged() {
        if (isAdmitted && !isDischarged) {
            dischargeTime = time(nullptr);
            isDischarged  = true;
            cout << "Discharge time recorded." << endl;
        }
    }

    void set() {
        cout << "Enter patient name: ";
        getline(cin >> ws, info.name);
        if (info.cnic == 0) {
            cout << "Enter patient CNIC: ";
            cin >> info.cnic;
        }
        cout << "Enter disease: ";
        getline(cin >> ws, info.disease);
        
    }

    void patientHistory() {
        cout << "Patient ID:     " << info.pID     << endl;
        cout << "Name:           " << info.name    << endl;
        cout << "CNIC:           " << info.cnic    << endl;
        cout << "Disease:        " << info.disease  << endl;
        cout << "Bed Charge/Day: " << admitBedCharge << endl;
        cout << "Admit Date:     " << asctime(&dateTime); 
        if (isDischarged) {
            struct tm dischargeTm = *localtime(&dischargeTime);
            cout << "Discharge Date: " << asctime(&dischargeTm);
        } else {
            cout << "Discharge Date: Still admitted" << endl;
        }
    }

    
    void addMedication(MedManagement &medManager) {
        if (isDischarged) {
            cout << "Cannot add medication — patient already discharged." << endl;
            return;
        }
        int medId = 0, qty = 0;
        cout << "Enter medicine ID: ";
        cin >> medId;
        cout << "Enter quantity: ";
        cin >> qty;
        Medication med;
        if (medManager.consumeMedicine(medId, qty, med)) {
            meds.push_back(med);
            cout << "Medication added." << endl;
        } else {
            cout << "Unable to add medication (invalid ID or insufficient stock)." << endl;
            return;
        }
    }

    void displayMedication() {
        if (meds.empty()) { cout << "No medications given." << endl; return; }
        for (int i = 0; i < (int)meds.size(); i++) {
            const Medication &m = meds[i];
            cout << "- " << m.name << " (" << m.type << "), qty=" << m.quantity
                 << ", unit price=" << m.price << endl;
        }
    }

    
    
    
    void generateBill() {
        if (!isAdmitted) { cout << "Patient has no admit record." << endl; return; }
        time_t endTime = (dischargeTime != 0) ? dischargeTime : time(nullptr);
        int days = (int)(difftime(endTime, admitTime) / 86400); 
        if (days < 1) days = 1;
        cout << "Days stayed: " << days << endl;

        double medCharges = 0.0;
        for (int i = 0; i < (int)meds.size(); i++)
            medCharges += meds[i].price * (double)meds[i].quantity;

        double bedCharges = admitBedCharge * (double)days; 
        double total      = medCharges + bedCharges;
        cout << "\n--- Bill Summary ---" << endl;
        cout << "Medication Charges: " << medCharges << endl;
        cout << "Bed Charges:        " << bedCharges << endl;
        cout << "Total:              " << total       << endl;
    }

};

int Patient::nextId = 0;

class PatientManagement {
private:
    vector<Patient>   patients;
    BedManagement    &bedManager;  
    MedManagement    &medManager;  
    
    void savePatientsToFile(){
    	ofstream file("patients.txt");
    	if(!file.is_open()){
    		cout<<"Error: could not open patients.txt for writting."<<endl;
    		return;
		}
		file<<Patient::getNextId()<<"\n";
		for(int i = 0; i < patients.size(); i++){
			const Patient &p = patients[i];
			const PatientInfo &inf = p.getInfo();
			file<<inf.pID<<"|"<<inf.name<<"|"<<inf.cnic<<"|"<<inf.disease<<"|"<<p.getIsAdmitted()<<"|"<<p.getIsDischarged()<<"|"<<p.getAdmitTime()<<"|"<<p.getDischargeTime()<<"|"<<p.getAdmitBedCharges()<<"\n";
		}
		file.close();
	}
	void saveMedicationsToFile(){
		ofstream file("medications.txt");
		if(!file.is_open()){
			cout<<"Error: could not open medication.txt for writting."<<endl;
			return;
		}
		for(int i = 0; i < (int)patients.size(); i++){
			const Patient &p = patients[i];
			const vector<Medication> &meds = p.getMeds();
			for(int j = 0; j < (int)meds.size(); j++){
				file<<p.id()<<"|"<<meds[j].medicineId<<"|"<<meds[j].name<<"|"<<meds[j].type<<"|"<<meds[j].quantity<<"|"<<meds[j].price<<"\n";
			}
		}
		file.close();
	}
    
    int findIndexByCnic(long long cnic) {
        for (int i = 0; i < (int)patients.size(); ++i)
            if (patients[i].getCnic() == cnic) return i;
        return -1;
    }

    
    Patient* findByCnic(long long cnic) {
        for (int i = 0; i < (int)patients.size(); ++i)
            if (patients[i].getCnic() == cnic) return &patients[i];
        return nullptr;
    }

public:
    PatientManagement(BedManagement &b, MedManagement &m) : bedManager(b), medManager(m) {
        cout << "PatientManagement Constructor Called" << endl;
    }

	void loadPatientsFromFile(){
		ifstream file("patients.txt");
		if(!file.is_open()){
			cout<<"No patients file found. Starting fresh"<<endl;
			return;
		}
		patients.clear();
		string line;
		if(getline(file,line)){
			if(!line.empty()){
				Patient::setNextId(stoi(line));
			}
		}
		while(getline(file,line)){
			if(line.empty()){
				continue;
			}
			stringstream ss(line);
			string token;
			Patient p;
			
			getline(ss,token,'|');
			int pid = stoi(token);
			getline(ss,token,'|');
			p.setName(token);
			getline(ss,token,'|');
			p.setCnic(stoll(token));
			getline(ss,token,'|');
			p.setDisease(token);
			getline(ss,token,'|');
			bool admitted = stoi(token);
			getline(ss,token,'|');
			bool discharged = stoi(token);
			getline(ss,token,'|');
			time_t aTime = (time_t)stoll(token);
			getline(ss,token,'|');
			time_t dTime = (time_t)stoll(token);
			getline(ss,token,'|');
			double charge = stod(token);
			
			p.restoreState(pid,admitted,discharged,aTime,dTime,charge);
			patients.push_back(p);
		}
		file.close();
		cout<<"Patients Loaded: "<<patients.size()<<endl;
	}
    void loadMedicationFromFile(){
    	ifstream file("medications.txt"); 	
    	if(!file.is_open()){
    		cout<<"No medication file found. Starting fresh,"<<endl;
    		return;
		}
		string line;
		while(getline(file,line)){
			if(line.empty()){
				continue;
			}
			stringstream ss(line);
			string token;
			Medication med;
			int pid = 0;
			getline(ss,token,'|');
			pid = stoi(token);
			getline(ss,token,'|');
			med.medicineId = stoi(token);
			getline(ss,token,'|');
			med.name = token;
			getline(ss,token,'|');
			med.type = token;
			getline(ss,token,'|');
			med.quantity = stoi(token);
			getline(ss,token,'|');
			med.price = stod(token);
			
			for(int i = 0; i<(int)patients.size(); i++){
				if(patients[i].id() == pid){
					patients[i].addMedicationDirect(med);
					break;
				}
			}
		}
		file.close();
		cout<<"Medications Loaded."<<endl;
	}

    void addPatient() {
        long long cnic = 0;  
        cout << "\n[New Patient Admission]\nFollow the prompts to admit a patient.\n";
        cout << "Enter patient CNIC: ";
        cin >> cnic;
        if (findIndexByCnic(cnic) != -1) {
            cout << "Patient with this CNIC already exists." << endl;
            return;
        }

        Patient p;
        p.setCnic(cnic);
        p.set(); 

        
        
        
        double assignedCharge = 0.0;
        
        
        if (!bedManager.assignBed(0, assignedCharge)) {
            cout << "No bed available. Patient not admitted." << endl;
            return;
        }
        
        bedManager.resignBed(0);

        
        p.markAdmitted(assignedCharge); 

        
        double dummy = 0.0;
        bedManager.assignBed(p.id(), dummy);

        patients.push_back(p);
        savePatientsToFile();
        saveMedicationsToFile();
        cout << "Patient added successfully." << endl;
    }

    void dischargePatient() {
        long long cnic = 0;
        cout << "\n[Patient Discharge]\nFollow the prompts to discharge a patient.\n";
        cout << "Enter patient CNIC to discharge: ";
        cin >> cnic;
        
        Patient *p = findByCnic(cnic);
        if (!p) { cout << "Patient not found." << endl; return; }
        if (p->getIsDischarged()) { cout << "Patient already discharged." << endl; return; }

        
        
        bedManager.resignBed(p->id());
        p->markDischarged();
        savePatientsToFile();

        cout << "\n--- Final Bill ---" << endl;
        p->generateBill(); 
        cout << "Patient discharged. Record retained for reference." << endl;
    }

    void removePatient() {
        long long cnic = 0;
        cout << "\n[Remove Patient Record]\n";
        cout << "Enter patient CNIC to remove record: ";
        cin >> cnic;
        int idx = findIndexByCnic(cnic);
        if (idx == -1) { cout << "Patient not found." << endl; return; }
        if (!patients[idx].getIsDischarged()) {
            cout << "Warning: patient not yet discharged. Force remove? (y/n): ";
            char c; cin >> c;
            if (c != 'y' && c != 'Y') return;
            bedManager.resignBed(patients[idx].id());
        }
        patients.erase(patients.begin() + idx);
        savePatientsToFile();
        saveMedicationsToFile();
        cout << "Patient record removed." << endl;
    }

    void managePatient() {
        long long cnic = 0;
        cout << "\n[Manage Existing Patient]\n";
        cout << "Enter patient CNIC: ";
        cin >> cnic;
        if (!findByCnic(cnic)) { cout << "Patient not found." << endl; return; }

        int choice = 0;
        while (choice != 5) {
            
            Patient *p = findByCnic(cnic);
            if (!p) { cout << "Patient record no longer exists." << endl; return; }
            cout << "\n[Patient Menu]\n1. Show history\n2. Add medication\n"
                    "3. Display medications\n4. Generate bill\n5. Back\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: p->patientHistory();          break;
                case 2: p->addMedication(medManager);
                		saveMedicationsToFile();
						break;
                case 3: p->displayMedication();       break;
                case 4: p->generateBill();            break;
                case 5:                               break;
                default: cout << "Invalid choice." << endl;
            }
        }
    }

    void patientManagementMenu() {
        int choice = 0;
        while (choice != 5) {
            cout << "\n[Patient Management]\n1. Add patient\n2. Discharge patient\n"
                    "3. Remove patient record\n4. Manage patient\n5. Back\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: addPatient();       break;
                case 2: dischargePatient(); break;
                case 3: removePatient();    break;
                case 4: managePatient();    break;
                case 5:                     break;
                default: cout << "Invalid choice." << endl;
            }
        }
    }
};

struct UserLogin {
    int    uID;
    string pwd;
};

class HMS {
private:
    UserLogin         user;
    BedManagement     bedManager;
    MedManagement     medManager;
    PatientManagement patientManager; 

	string getPassword(){
		string pwd = "";
		int ch;
		cout << "(Password will be hidden as * while typing)" << endl;
		while(true){
			ch = _getch();
			if(ch == '\r' || ch == '\n'){
				break;
			}
			else if(ch == 0 || ch == 224){
				_getch();
			}
			else if(ch == 8 || ch == 127){
				if(!pwd.empty()){
					pwd.pop_back();
					cout<<"\b \b"<<flush;
				}
			}
			else if(ch >= 32 && ch < 127){
				pwd += ch;
				cout<<"*"<<flush;
			}
		}
		cout<<endl;
		return pwd;
	}
public:
    HMS(int defaultUserId = 4040, string defaultPassword = "Admin")
        : patientManager(bedManager, medManager) 
    {
        user.uID = defaultUserId;
        user.pwd = defaultPassword;
        cout << "HMS Constructor Called" << endl;
    	
    	bedManager.loadFromFile();
    	medManager.loadFromFile();
    	patientManager.loadPatientsFromFile();
    	patientManager.loadMedicationFromFile();
	}

    
    bool Login() {
        int    tries      = 3;
        string uInputPwd  = ""; 
        int    uInputId   = 0;
        cout << "\n=== HMS Login ===" << endl;
        cout << "Use your assigned ID and password to continue." << endl;
        while (tries > 0) {
            cout << "Attempts remaining: " << tries << endl;
            cout << "Enter ID: ";
            cin >> uInputId;
            cout << "Enter Password: ";
            uInputPwd = getPassword();
            if (uInputId == user.uID && uInputPwd == user.pwd) {
                cout << "Login successful." << endl;
                return true;
                }
            --tries;
            if (tries > 0) cout << "Incorrect. Try again." << endl;
        }
        cout << "Session locked after too many failed attempts." << endl;
        return false; 
    }

    void menu() {
        if (!Login()) return;
        int choice = 0;
        cout << "\nSystem ready. Choose a module from the main menu." << endl;
        while (choice != 4) {
            cout << "\nWelcome to Hospital Management System\n"
                    "1. Patient Management\n2. Medicine Management\n"
                    "3. Bed Management\n4. Exit\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: patientManager.patientManagementMenu(); break;
                case 2: medManager.medicineMenu();              break;
                case 3: bedManager.bedMenu();                   break;
                case 4: cout << "Goodbye." << endl;             break;
                default: cout << "Invalid choice." << endl;
            }
        }
    }
};

int main() {
    HMS obj;
    obj.menu();
    return 0;
}
