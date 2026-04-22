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
    long long cnic;

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
    }

    bool assignBed(int patientId) {
        for (int i = 0; i < (int)beds.size(); i++) {
            Bed &bed = beds[i];
            if (!bed.reserved) {
                bed.reserved = true;
                bed.pBed = patientId;
                cout << "Bed " << bed.bId << " assigned to patient #" << patientId << "\n";
                return true;
            }
        }
        cout << "No bed available.\n";
        return false;
    }

    void resignBed(int patientId) {
        for (int i = 0; i < (int)beds.size(); i++) {
            Bed &bed = beds[i];
            if (bed.reserved && bed.pBed == patientId) {
                bed.reserved = false;
                bed.pBed = 0;
                cout << "Bed " << bed.bId << " released.\n";
                return;
            }
        }
    }

    int currentBedCharges() const {
        if (beds.empty()) return 0;
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
            if (toAdd <= 0) {
                cout << "Invalid number.\n";
                return;
            }
            int start = (int)beds.size();
            for (int i = 0; i < toAdd; ++i) {
                beds.push_back({start + i, 0, false, currentBedCharges()});
            }
            cout << "Total beds: " << beds.size() << "\n";
        } else if (choice == 2) {
            int newCharge = 0;
            cout << "Enter new bed charge: ";
            cin >> newCharge;
            if (newCharge < 0) {
                cout << "Bed charge cannot be negative.\n";
                return;
            }
            for (int i = 0; i < (int)beds.size(); i++) {
                beds[i].bedCharges = newCharge;
            }
            cout << "Updated bed charges to " << newCharge << "\n";
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void get() const {
        int choice = 0;
        cout << "Enter 0 for free beds, 1 for reserved beds, 2 for total beds: ";
        cin >> choice;

        switch (choice) {
            case 0:
                for (int i = 0; i < (int)beds.size(); i++) {
                    const Bed &bed = beds[i];
                    if (!bed.reserved) cout << "Bed " << bed.bId << " is available.\n";
                }
                break;
            case 1:
                for (int i = 0; i < (int)beds.size(); i++) {
                    const Bed &bed = beds[i];
                    if (bed.reserved) cout << "Bed " << bed.bId << " is reserved by patient #" << bed.pBed << ".\n";
                }
                break;
            case 2:
                cout << "Total beds: " << beds.size() << "\n";
                break;
            default:
                cout << "Invalid choice.\n";
        }
    }

    void removeBed() {
        int id = 0;
        cout << "Enter bed ID to remove: ";
        cin >> id;
        if (id < 0 || id >= (int)beds.size()) {
            cout << "Invalid bed ID.\n";
            return;
        }
        if (beds[id].reserved) {
            cout << "Cannot remove reserved bed.\n";
            return;
        }
        beds.erase(beds.begin() + id);
        for (int i = 0; i < (int)beds.size(); ++i) beds[i].bId = i;
        cout << "Bed removed.\n";
    }

    void bedMenu() {
        int choice = 0;
        while (choice != 4) {
            cout << "\n[Bed Management]\n"
                 << "1. Add/Update beds\n"
                 << "2. View beds\n"
                 << "3. Remove bed\n"
                 << "4. Back\n"
                 << "Choice: ";
            cin >> choice;
            switch (choice) {
                case 1: set(); break;
                case 2: get(); break;
                case 3: removeBed(); break;
                case 4: break;
                default: cout << "Invalid choice.\n";
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
    }

    void set() {
        char more = 'y';
        while (more == 'y' || more == 'Y') {
            Medicine m;
            m.mID = (int)medics.size();
            m.name = "";
            m.type = "";
            m.stock = 0;
            m.pricePerItem = 0.0F;
            cout << "Enter medicine type: ";
            cin >> m.type;
            cout << "Enter medicine name: ";
            cin >> m.name;
            cout << "Enter price per item: ";
            cin >> m.pricePerItem;
            if (m.pricePerItem < 0) {
                cout << "Price cannot be negative.\n";
                continue;
            }
            cout << "Enter stock: ";
            cin >> m.stock;
            if (m.stock < 0) {
                cout << "Stock cannot be negative.\n";
                continue;
            }
            medics.push_back(m);

            cout << "Add more medicines? (y/n): ";
            cin >> more;
        }
    }

    void get() const {
        if (medics.empty()) {
            cout << "No medicines available.\n";
            return;
        }
        for (int i = 0; i < (int)medics.size(); i++) {
            const Medicine &m = medics[i];
            cout << "ID: " << m.mID << ", Name: " << m.name << ", Type: " << m.type
                 << ", Stock: " << m.stock << ", Price: " << m.pricePerItem << "\n";
        }
    }

    void removeItem() {
        int id = 0;
        cout << "Enter medicine ID to remove: ";
        cin >> id;
        if (id < 0 || id >= (int)medics.size()) {
            cout << "Invalid ID.\n";
            return;
        }
        medics.erase(medics.begin() + id);
        for (int i = 0; i < (int)medics.size(); ++i) medics[i].mID = i;
        cout << "Medicine removed.\n";
    }

    void updateStock() {
        int id = 0, add = 0;
        cout << "Enter medicine ID: ";
        cin >> id;
        if (id < 0 || id >= (int)medics.size()) {
            cout << "Invalid ID.\n";
            return;
        }
        cout << "Enter stock to add: ";
        cin >> add;
        if (add < 0) {
            cout << "Stock increment cannot be negative.\n";
            return;
        }
        medics[id].stock += add;
        cout << "New stock for " << medics[id].name << ": " << medics[id].stock << "\n";
    }

    bool consumeMedicine(int id, int qty, Medication &out) {
        if (id < 0 || id >= (int)medics.size()) return false;
        if (qty <= 0 || medics[id].stock < qty) return false;

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
            cout << "\n[Medicine Management]\n"
                 << "1. Add medicines\n"
                 << "2. Display medicines\n"
                 << "3. Remove medicine\n"
                 << "4. Update stock\n"
                 << "5. Back\n"
                 << "Choice: ";
            cin >> choice;
            switch (choice) {
                case 1: set(); break;
                case 2: get(); break;
                case 3: removeItem(); break;
                case 4: updateStock(); break;
                case 5: break;
                default: cout << "Invalid choice.\n";
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
    Patient(long long presetCnic = 0, const string &presetName = "", const string &presetDisease = "",
            const string &presetDate = "") {
        info.pID = ++nextId;
        info.cnic = presetCnic;
        info.name = presetName;
        info.disease = presetDisease;
        info.dateAdmit = presetDate;
    }

    int id() const { return info.pID; }
    long long cnic() const { return info.cnic; }
    void setCnic(long long value) { info.cnic = value; }

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
        cout << "Assigned patient ID: " << info.pID << "\n";
    }

    void patientHistory() const {
        cout << "Patient ID: " << info.pID << "\n"
             << "Name: " << info.name << "\n"
             << "CNIC: " << info.cnic << "\n"
             << "Disease: " << info.disease << "\n"
             << "Admit Date: " << info.dateAdmit << "\n";
    }

    void addMedication(MedManagement &medManager) {
        int medId = 0, qty = 0;
        cout << "Enter medicine ID: ";
        cin >> medId;
        cout << "Enter quantity: ";
        cin >> qty;

        Medication med;
        if (medManager.consumeMedicine(medId, qty, med)) {
            meds.push_back(med);
            cout << "Medication added to patient.\n";
        } else {
            cout << "Unable to add medication (invalid ID or insufficient stock).\n";
        }
    }

    void displayMedication() const {
        if (meds.empty()) {
            cout << "No medications given.\n";
            return;
        }
        for (int i = 0; i < (int)meds.size(); i++) {
            const Medication &m = meds[i];
            cout << "- " << m.name << " (" << m.type << "), qty=" << m.quantity
                 << ", unit price=" << m.price << "\n";
        }
    }

    void generateBill(int bedChargePerDay) const {
        int days = 0;
        cout << "Enter number of days stayed: ";
        cin >> days;
        if (days < 0) {
            cout << "Days cannot be negative.\n";
            return;
        }

        float medCharges = 0.0F;
        for (int i = 0; i < (int)meds.size(); i++) {
            medCharges += meds[i].price * meds[i].quantity;
        }
        float bedCharges = (float)(bedChargePerDay * days);
        float total = medCharges + bedCharges;

        cout << "\nBill Summary\n"
             << "Medication Charges: " << medCharges << "\n"
             << "Bed Charges: " << bedCharges << "\n"
             << "Total: " << total << "\n";
    }
};

int Patient::nextId = 0;

BedManagement &defaultBedManager() {
    static BedManagement instance;
    return instance;
}

MedManagement &defaultMedManager() {
    static MedManagement instance;
    return instance;
}

class PatientManagement {
private:
    vector<Patient> patients;
    BedManagement &bedManager;
    MedManagement &medManager;

    int findIndexByCnic(long long cnic) const {
        for (int i = 0; i < (int)patients.size(); ++i) {
            if (patients[i].cnic() == cnic) return i;
        }
        return -1;
    }

public:
    PatientManagement(BedManagement &b = defaultBedManager(), MedManagement &m = defaultMedManager())
        : bedManager(b), medManager(m) {}

    void addPatient() {
        long long cnic = 0;
        cout << "Enter patient CNIC: ";
        cin >> cnic;
        if (findIndexByCnic(cnic) != -1) {
            cout << "Patient with this CNIC already exists.\n";
            return;
        }

        Patient p;
        p.setCnic(cnic);
        p.set();
        if (!bedManager.assignBed(p.id())) {
            cout << "Patient not added because no bed is available.\n";
            return;
        }
        patients.push_back(p);
        cout << "Patient added successfully.\n";
    }

    void removePatient() {
        long long cnic = 0;
        cout << "Enter patient CNIC to remove: ";
        cin >> cnic;
        int idx = findIndexByCnic(cnic);
        if (idx == -1) {
            cout << "Patient not found.\n";
            return;
        }

        bedManager.resignBed(patients[idx].id());
        patients.erase(patients.begin() + idx);
        cout << "Patient removed successfully.\n";
    }

    void managePatient() {
        long long cnic = 0;
        cout << "Enter patient CNIC: ";
        cin >> cnic;
        int idx = findIndexByCnic(cnic);
        if (idx == -1) {
            cout << "Patient not found.\n";
            return;
        }

        int choice = 0;
        while (choice != 5) {
            cout << "\n[Patient Menu]\n"
                 << "1. Show history\n"
                 << "2. Add medication\n"
                 << "3. Display medications\n"
                 << "4. Generate bill\n"
                 << "5. Back\n"
                 << "Choice: ";
            cin >> choice;
            switch (choice) {
                case 1: patients[idx].patientHistory(); break;
                case 2: patients[idx].addMedication(medManager); break;
                case 3: patients[idx].displayMedication(); break;
                case 4: patients[idx].generateBill(bedManager.currentBedCharges()); break;
                case 5: break;
                default: cout << "Invalid choice.\n";
            }
        }
    }

    void patientManagementMenu() {
        int choice = 0;
        while (choice != 4) {
            cout << "\n[Patient Management]\n"
                 << "1. Add patient\n"
                 << "2. Remove patient\n"
                 << "3. Manage patient\n"
                 << "4. Back\n"
                 << "Choice: ";
            cin >> choice;
            switch (choice) {
                case 1: addPatient(); break;
                case 2: removePatient(); break;
                case 3: managePatient(); break;
                case 4: break;
                default: cout << "Invalid choice.\n";
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
    vector<UserLogin> loginInfo;
    BedManagement bedManager;
    MedManagement medManager;
    PatientManagement patientManager;

public:
    HMS(int defaultUserId = 4040, const string &defaultPassword = "Admin")
        : patientManager(bedManager, medManager) {
        UserLogin user;
        user.uID = defaultUserId;
        user.pwd = defaultPassword;
        loginInfo.push_back(user);
    }

    void menu() {
        int choice = 0;
        while (choice != 4) {
            cout << "\nWelcome to Hospital Management System\n"
                 << "1. Patient Management\n"
                 << "2. Medicine Management\n"
                 << "3. Bed Management\n"
                 << "4. Exit\n"
                 << "Choice: ";
            cin >> choice;
            switch (choice) {
                case 1: patientManager.patientManagementMenu(); break;
                case 2: medManager.medicineMenu(); break;
                case 3: bedManager.bedMenu(); break;
                case 4: cout << "Goodbye.\n"; break;
                default: cout << "Invalid choice.\n";
            }
        }
    }
};

int main() {
    HMS obj;
    obj.menu();
    return 0;
}
