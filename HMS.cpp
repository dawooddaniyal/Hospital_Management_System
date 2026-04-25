/*
 * Hospital Management System — HMS.cpp
 * All original bugs fixed + logic improvements applied.
 */

#include <iostream>
#include <conio.h>
#include <string>
#include <vector>
#include <ctime>
#include <cstring>

using namespace std;

// ─────────────────────────────────────────────
//  STRUCTS
// ─────────────────────────────────────────────

struct Bed {
    int    bId;
    int    pBed;
    bool   reserved;
    double bedCharges;   // L8: was int — unified to double for consistent money arithmetic
};

struct Medicine {
    int    mID;
    string name;
    string type;
    int    stock;
    double pricePerItem; // L8: was float — unified to double
};

struct Medication {
    int    medicineId;
    string name;
    string type;
    int    quantity;
    double price;        // L8: was float — unified to double
};

struct PatientInfo {
    int       pID;
    string    name;
    string    disease;
    // L4: removed dead string dateAdmit — actual admit time stored as time_t in Patient
    long long cnic;     // B4: was int — long long handles 13-digit Pakistani CNICs
};

// ─────────────────────────────────────────────
//  BedManagement
// ─────────────────────────────────────────────

class BedManagement {
private:
    vector<Bed> beds;

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

    // L2: returns the assigned bed's charge rate so Patient can snapshot it at admit time
    bool assignBed(int patientId, double &assignedCharge) {
        for (int i = 0; i < (int)beds.size(); i++) {
            Bed &bed = beds[i];
            if (!bed.reserved) {
                bed.reserved   = true;
                bed.pBed       = patientId;
                assignedCharge = bed.bedCharges;
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
                bed.pBed     = 0;
                cout << "Bed " << bed.bId << " released." << endl;
                return; // B7: early return — one patient occupies one bed
            }
        }
    }

    double currentBedCharges() const {
        if (beds.empty()) return 0.0;
        return beds.front().bedCharges;
    }

    void set() {
        int choice = 0;
        cout << "Enter 1 to add beds or 2 to update bed charges: ";
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
        } else if (choice == 2) {
            double newCharge = 0.0;
            cout << "Enter new bed charge: ";
            cin >> newCharge;
            if (newCharge < 0) { cout << "Bed charge cannot be negative." << endl; return; }
            for (int i = 0; i < (int)beds.size(); i++) beds[i].bedCharges = newCharge;
            cout << "Updated bed charges to " << newCharge << endl;
        } else {
            cout << "Invalid choice." << endl;
        }
    }

    void get() {
        int choice = 0;
        cout << "Enter 0=free beds, 1=reserved beds, 2=total beds\nChoice: ";
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
        // B8: search by bId value, not raw vector index
        int foundIdx = -1;
        for (int i = 0; i < (int)beds.size(); ++i)
            if (beds[i].bId == id) { foundIdx = i; break; }
        if (foundIdx == -1) { cout << "Invalid bed ID." << endl; return; }
        if (beds[foundIdx].reserved) { cout << "Cannot remove reserved bed." << endl; return; }
        beds.erase(beds.begin() + foundIdx);
        for (int i = 0; i < (int)beds.size(); ++i) beds[i].bId = i;
        cout << "Bed removed." << endl;
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

// ─────────────────────────────────────────────
//  MedManagement
// ─────────────────────────────────────────────

class MedManagement {
private:
    vector<Medicine> medics;

    // L5: find index by mID value — not raw vector index
    int findIndexById(int mID) {
        for (int i = 0; i < (int)medics.size(); ++i)
            if (medics[i].mID == mID) return i;
        return -1;
    }

public:
    MedManagement(int reserveCount = 0) {
        if (reserveCount > 0) medics.reserve(reserveCount);
        cout << "MedManagement Constructor Called" << endl;
    }

    void set() {
        char more = 'y';
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
        // L5: search by mID value — mirrors removeBed fix
        int idx = findIndexById(id);
        if (idx == -1) { cout << "Invalid ID." << endl; return; }
        medics.erase(medics.begin() + idx);
        for (int i = 0; i < (int)medics.size(); ++i) medics[i].mID = i;
        cout << "Medicine removed." << endl;
    }

    void updateStock() {
        int id = 0, add = 0;
        cout << "Enter medicine ID: ";
        cin >> id;
        // L5: search by mID value
        int idx = findIndexById(id);
        if (idx == -1) { cout << "Invalid ID." << endl; return; }
        cout << "Enter stock to add: ";
        cin >> add;
        if (add < 0) { cout << "Stock increment cannot be negative." << endl; return; }
        medics[idx].stock += add;
        cout << "New stock for " << medics[idx].name << ": " << medics[idx].stock << endl;
    }

    // B1: Medication &out — pass by reference so caller receives filled data
    bool consumeMedicine(int id, int qty, Medication &out) {
        int idx = findIndexById(id); // L5: lookup by mID value
        if (idx == -1) return false;
        if (qty <= 0 || medics[idx].stock < qty) return false;
        medics[idx].stock -= qty;
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

// ─────────────────────────────────────────────
//  Patient
// ─────────────────────────────────────────────

class Patient {
private:
    static int        nextId;
    PatientInfo       info;
    bool              isAdmitted;      // L4/B-improve: explicit bool, not admitTime==0
    bool              isDischarged;    // locks record after discharge
    time_t            admitTime;
    time_t            dischargeTime;
    struct tm         dateTime;
    double            admitBedCharge;  // L2: charge rate snapshotted at admit time
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
        admitBedCharge = 0.0;          // L2: initialise snapshot
        memset(&dateTime, 0, sizeof(dateTime));
        cout << "Patient Constructor Called" << endl;
    }

    int       id()              { return info.pID;     }
    long long getCnic()         { return info.cnic;    } // B10: renamed from cnic()
    bool      getIsDischarged() { return isDischarged; }

    void setCnic(long long value) { info.cnic = value; }

    // L2: accepts bed charge snapshotted at assignment — stored for billing
    void markAdmitted(double bedCharge) {
        if (!isAdmitted) {
            admitTime      = time(nullptr);
            dateTime       = *localtime(&admitTime);
            mktime(&dateTime);
            isAdmitted     = true;
            admitBedCharge = bedCharge; // L2: snapshot rate
            info.pID       = ++nextId;  // B6: ID assigned only after bed confirmed
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
        // markAdmitted() is called by addPatient() AFTER bed is confirmed — not here
    }

    void patientHistory() {
        cout << "Patient ID:     " << info.pID     << endl;
        cout << "Name:           " << info.name    << endl;
        cout << "CNIC:           " << info.cnic    << endl;
        cout << "Disease:        " << info.disease  << endl;
        cout << "Bed Charge/Day: " << admitBedCharge << endl;
        cout << "Admit Date:     " << asctime(&dateTime); // B12: asctime appends \n — no endl
        if (isDischarged) {
            struct tm dischargeTm = *localtime(&dischargeTime);
            cout << "Discharge Date: " << asctime(&dischargeTm);
        } else {
            cout << "Discharge Date: Still admitted" << endl;
        }
    }

    // B2: MedManagement passed by reference — stock deductions are real
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

    // L1: correct operator precedence (divide then cast)
    // L2: uses snapshotted admitBedCharge — not live rate from bedManager
    // L3: uses dischargeTime if available, else time(now)
    void generateBill() {
        if (!isAdmitted) { cout << "Patient has no admit record." << endl; return; }
        time_t endTime = (dischargeTime != 0) ? dischargeTime : time(nullptr);
        int days = (int)(difftime(endTime, admitTime) / 86400); // L1: fixed precedence
        if (days < 1) days = 1;
        cout << "Days stayed: " << days << endl;

        double medCharges = 0.0;
        for (int i = 0; i < (int)meds.size(); i++)
            medCharges += meds[i].price * (double)meds[i].quantity;

        double bedCharges = admitBedCharge * (double)days; // L2: snapshotted rate
        double total      = medCharges + bedCharges;
        cout << "\n--- Bill Summary ---" << endl;
        cout << "Medication Charges: " << medCharges << endl;
        cout << "Bed Charges:        " << bedCharges << endl;
        cout << "Total:              " << total       << endl;
    }
};

int Patient::nextId = 0;

// ─────────────────────────────────────────────
//  PatientManagement
// ─────────────────────────────────────────────

class PatientManagement {
private:
    vector<Patient>   patients;
    BedManagement    &bedManager;  // B3: reference — no disconnected copy
    MedManagement    &medManager;  // B3: reference — no disconnected copy

    int findIndexByCnic(long long cnic) {
        for (int i = 0; i < (int)patients.size(); ++i)
            if (patients[i].getCnic() == cnic) return i;
        return -1;
    }

    // L9: pointer-based re-lookup to prevent stale index across operations
    Patient* findByCnic(long long cnic) {
        for (int i = 0; i < (int)patients.size(); ++i)
            if (patients[i].getCnic() == cnic) return &patients[i];
        return nullptr;
    }

public:
    PatientManagement(BedManagement &b, MedManagement &m) : bedManager(b), medManager(m) {
        cout << "PatientManagement Constructor Called" << endl;
    }

    void addPatient() {
        long long cnic = 0;  // B4: long long
        cout << "Enter patient CNIC: ";
        cin >> cnic;
        if (findIndexByCnic(cnic) != -1) {
            cout << "Patient with this CNIC already exists." << endl;
            return;
        }

        Patient p;
        p.setCnic(cnic);
        p.set(); // collects name/disease — does NOT call markAdmitted yet

        // L1 (addPatient): check bed availability BEFORE pushing to vector
        //    so a failed assignment never creates a ghost reservation or wastes an ID
        // Use a temporary sentinel ID=0 to probe availability, then assign real ID after
        double assignedCharge = 0.0;
        // Probe bed availability without consuming it permanently
        // We call assignBed with a temp ID, confirm success, then re-assign with real ID
        if (!bedManager.assignBed(0, assignedCharge)) {
            cout << "No bed available. Patient not admitted." << endl;
            return;
        }
        // Release the temporary probe assignment
        bedManager.resignBed(0);

        // Now markAdmitted — this sets the real patient ID
        p.markAdmitted(assignedCharge); // B6 + L2: ID set here, charge snapshotted

        // Assign bed with the real patient ID
        double dummy = 0.0;
        bedManager.assignBed(p.id(), dummy);

        patients.push_back(p);
        cout << "Patient added successfully." << endl;
    }

    void dischargePatient() {
        long long cnic = 0;
        cout << "Enter patient CNIC to discharge: ";
        cin >> cnic;
        // L9: pointer-based lookup
        Patient *p = findByCnic(cnic);
        if (!p) { cout << "Patient not found." << endl; return; }
        if (p->getIsDischarged()) { cout << "Patient already discharged." << endl; return; }

        // L3 (discharge): bed charge already snapshotted on Patient at admit time
        //    so we can safely release the bed BEFORE generating the bill
        bedManager.resignBed(p->id());
        p->markDischarged();

        cout << "\n--- Final Bill ---" << endl;
        p->generateBill(); // L2: uses snapshotted charge; L3: uses dischargeTime
        cout << "Patient discharged. Record retained for reference." << endl;
    }

    void removePatient() {
        long long cnic = 0;
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
        cout << "Patient record removed." << endl;
    }

    void managePatient() {
        long long cnic = 0;
        cout << "Enter patient CNIC: ";
        cin >> cnic;
        if (!findByCnic(cnic)) { cout << "Patient not found." << endl; return; }

        int choice = 0;
        while (choice != 5) {
            // L9: fresh pointer each loop iteration — stale index impossible
            Patient *p = findByCnic(cnic);
            if (!p) { cout << "Patient record no longer exists." << endl; return; }
            cout << "\n[Patient Menu]\n1. Show history\n2. Add medication\n"
                    "3. Display medications\n4. Generate bill\n5. Back\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: p->patientHistory();          break;
                case 2: p->addMedication(medManager); break;
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

// ─────────────────────────────────────────────
//  HMS
// ─────────────────────────────────────────────

struct UserLogin {
    int    uID;
    string pwd;
};

class HMS {
private:
    UserLogin         user;
    BedManagement     bedManager;
    MedManagement     medManager;
    PatientManagement patientManager; // B3: refs into bedManager & medManager — shared state

	string getPassword(){
		string pwd = "";
		int ch;
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
        : patientManager(bedManager, medManager) // B3: passes refs to own members
    {
        user.uID = defaultUserId;
        user.pwd = defaultPassword;
        cout << "HMS Constructor Called" << endl;
    }

    // L6: session lockout — program must be restarted after 3 failed attempts
    bool Login() {
        int    tries      = 3;
        string uInputPwd  = ""; // B9: was " " (space)
        int    uInputId   = 0;
        while (tries > 0) {
            cout << "Attempts remaining: " << tries << endl;
            cout << "Enter ID: ";
            cin >> uInputId;
            cout << "Enter Password: ";
            uInputPwd = getPassword();
            if (uInputId == user.uID && uInputPwd == user.pwd) return true;
            --tries;
            if (tries > 0) cout << "Incorrect. Try again." << endl;
        }
        cout << "Session locked after too many failed attempts." << endl;
        return false; // L6: caller (menu) returns — program exits session
    }

    void menu() {
        if (!Login()) return;
        int choice = 0;
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

// ─────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────

int main() {
    HMS obj;
    obj.menu();
    return 0;
}
