// ============================================================
//  Hospital Management System (HMS)
//  Manages patients, beds, and medicines for a hospital.
//  Data is persisted to disk after every change using flat
//  pipe-delimited text files (beds.txt, medicines.txt,
//  patients.txt, medications.txt, ).
//
//  Author : [Dawood Imran]
//  Date   : 2025
// ============================================================

#include <iostream>
#include <conio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstring>

using namespace std;

// ------------------------------------------------------------
//  HELPER — prints a full-width divider line to the console
// ------------------------------------------------------------
void printLine() {
	cout << "  ----------------------------------------" << endl;
}

// ------------------------------------------------------------
//  HELPER — prints a centered section header with borders
// ------------------------------------------------------------
void printHeader(const string &title) {
	cout << "\n  ========================================" << endl;
	cout << "           " << title                        << endl;
	cout << "  ========================================" << endl;
}

// ============================================================
//  CLASS: Validator
//  Provides validated input helpers for all user-facing fields.
//  All methods loop until the user enters acceptable input.
// ============================================================
class Validator {
	public:
		Validator() {
			cout << "Validator Constructor called" << endl;
		}

		// Returns a valid 13-digit numeric CNIC
		static long long getCnic() {
			string input;
			while(true) {
				getline(cin >> ws, input);
				if(input.size() != 13) {
					cout << "  [Error] CNIC must be exactly 13 digits. Try again: ";
					continue;
				}
				bool valid = true;
				for(int i = 0; i < (int)input.size(); i++) {
					if(!isdigit(input[i])) {
						valid = false;
						break;
					}
				}
				if(!valid) {
					cout << "  [Error] CNIC must contain digits only. Try again: ";
					continue;
				}
				return stoll(input);
			}
		}

		// Returns a non-empty string containing only letters and spaces
		static string getTextOnly() {
			string input;
			while(true) {
				getline(cin >> ws, input);
				if(input.empty()) {
					cout << "  [Error] Field cannot be empty. Try again: ";
					continue;
				}
				bool valid = true;
				for(int i = 0; i < (int)input.size(); i++) {
					if(!isalpha(input[i]) && input[i] != ' ') {
						valid = false;
						break;
					}
				}
				if(!valid) {
					cout << "  [Error] Only letters and spaces are allowed. Try again: ";
					continue;
				}
				return input;
			}
		}

		// Returns a non-empty string for disease names (letters, digits, spaces, hyphens allowed)
		static string getDiagnosis() {
			string input;
			while(true) {
				getline(cin >> ws, input);
				if(input.empty()) {
					cout << "  [Error] Disease field cannot be empty. Try again: ";
					continue;
				}
				bool valid = true;
				for(int i = 0; i < (int)input.size(); i++) {
					if(!isalpha(input[i]) && !isdigit(input[i]) && input[i] != ' ' && input[i] != '-') {
						valid = false;
						break;
					}
				}
				if(!valid) {
					cout << "  [Error] Only letters, digits, spaces and hyphens are allowed. Try again: ";
					continue;
				}
				return input;
			}
		}

		// Returns a positive double (for prices and bed charges)
		static double getPositiveDouble() {
			double value;
			while(true) {
				if(cin >> value && value > 0) {
					return value;
				}
				cin.clear();
				cin.ignore(1000, '\n');
				cout << "  [Error] Please enter a positive number: ";
			}
		}

		// Returns a positive integer (for stock, quantity, bed count)
		static int getPositiveInt() {
			int value;
			while(true) {
				if(cin >> value && value > 0) {
					return value;
				}
				cin.clear();
				cin.ignore(1000, '\n');
				cout << "  [Error] Please enter a positive whole number: ";
			}
		}
};


// ============================================================
//  STRUCTS — plain data containers with no behaviour
// ============================================================

// Represents a single hospital bed
struct Bed {
	int    bId;        // unique bed identifier
	int    pBed;       // ID of patient currently in this bed (0 = empty)
	bool   reserved;   // true if bed is occupied
	double bedCharges; // cost per day for this bed
};

// Represents a medicine item in the pharmacy inventory
struct Medicine {
	int    mID;          // unique medicine identifier
	string name;         // medicine name
	string type;         // form of medicine e.g. Tablet, Syrup
	int    stock;        // units currently available
	double pricePerItem; // cost per unit
};

// Represents a single medicine given to a patient (snapshot at time of prescription)
struct Medication {
	int    medicineId; // ID of the source medicine
	string name;       // name at time of prescription
	string type;       // type at time of prescription
	int    quantity;   // number of units prescribed
	double price;      // unit price at time of prescription
};

// Holds the core identity information of a patient
struct PatientInfo {
	int       pID;     // unique patient identifier
	string    name;    // full name
	string    disease; // diagnosed condition
	long long cnic;    // national identity number (13 digits)
};


// ============================================================
//  CLASS: BedManagement
//  Owns and manages the hospital's bed inventory.
//  Persists all changes to beds.txt automatically.
// ============================================================
class BedManagement {
private:
	vector<Bed> beds;          // in-memory list of all beds
	int         defaultCount;  // number of beds to create on first run
	double      defaultCharge; // default per-day charge for new beds

	// Writes the full bed list to beds.txt, overwriting previous content
	void saveToFile() {
		ofstream file("beds.txt");
		if(!file.is_open()) {
			cout << "  [Error] Could not open beds.txt for writing." << endl;
			return;
		}
		for(int i = 0; i < (int)beds.size(); i++) {
			file << beds[i].bId        << "|"
			     << beds[i].pBed       << "|"
			     << beds[i].reserved   << "|"
			     << beds[i].bedCharges << "\n";
		}
		file.close();
	}

public:
	// Constructor — stores defaults, actual beds are created in loadFromFile()
	BedManagement(int initialBedCount = 10, double defaultBedCharge = 1000.0)
		: defaultCount(initialBedCount), defaultCharge(defaultBedCharge) {
		cout << "  [System] BedManagement initialised." << endl;
	}

	// Loads beds from beds.txt on startup.
	// On first run (no file), creates default beds and saves them.
	void loadFromFile() {
		ifstream file("beds.txt");
		if(!file.is_open()) {
			cout << "  [Info] No beds file found. Creating " << defaultCount << " default beds." << endl;
			for(int i = 0; i < defaultCount; ++i) {
				Bed bed;
				bed.bId        = i;
				bed.pBed       = 0;
				bed.reserved   = false;
				bed.bedCharges = defaultCharge;
				beds.push_back(bed);
			}
			saveToFile();
			return;
		}
		beds.clear();
		string line;
		// Read each line and reconstruct a Bed from pipe-separated fields
		while(getline(file, line)) {
			if(line.empty()) {
				continue;
			}
			stringstream ss(line);
			string token;
			Bed bed;
			getline(ss, token, '|'); bed.bId        = stoi(token);
			getline(ss, token, '|'); bed.pBed       = stoi(token);
			getline(ss, token, '|'); bed.reserved   = stoi(token);
			getline(ss, token, '|'); bed.bedCharges = stod(token);
			beds.push_back(bed);
		}
		file.close();
		cout << "  [Info] Beds loaded: " << beds.size() << endl;
	}

	// Scans for the first free bed and returns its index and charge.
	// Does NOT reserve the bed — caller decides when to commit.
	// Returns -1 if no free bed exists.
	int findFreeBed(double &charge) {
		for(int i = 0; i < (int)beds.size(); i++) {
			if(!beds[i].reserved) {
				charge = beds[i].bedCharges;
				return i;
			}
		}
		return -1;
	}

	// Reserves a specific bed (by vector index) for a given patient ID
	void assignBedByIndex(int idx, int patientId) {
		beds[idx].reserved = true;
		beds[idx].pBed     = patientId;
		cout << "  [Bed] Bed " << beds[idx].bId
		     << " assigned to patient #" << patientId << endl;
		saveToFile();
	}

	// Releases the bed currently held by the given patient ID
	void resignBed(int patientId) {
		for(int i = 0; i < (int)beds.size(); i++) {
			if(beds[i].reserved && beds[i].pBed == patientId) {
				beds[i].reserved = false;
				beds[i].pBed     = 0;
				cout << "  [Bed] Bed " << beds[i].bId << " released." << endl;
				saveToFile();
				return;
			}
		}
	}

	// Returns the per-day charge of the first bed in the list.
	// Used as the default charge when adding new beds.
	double currentBedCharges() const {
		if(beds.empty()) {
			return 0.0;
		}
		return beds.front().bedCharges;
	}

	// Allows admin to add new beds or update the charge for all beds
	void set() {
		printHeader("BED SETUP");
		cout << "  1. Add new beds" << endl;
		cout << "  2. Update per-day charge for all beds" << endl;
		printLine();
		cout << "  Choice: ";
		int choice = 0;
		cin >> choice;

		if(choice == 1) {
			cout << "  Enter number of beds to add: ";
			int toAdd = Validator::getPositiveInt();
			int start = (int)beds.size();
			for(int i = 0; i < toAdd; ++i) {
				beds.push_back({start + i, 0, false, currentBedCharges()});
			}
			cout << "  [OK] Total beds now: " << beds.size() << endl;
			saveToFile();
		} else if(choice == 2) {
			cout << "  Enter new charge per day: ";
			double newCharge = Validator::getPositiveDouble();
			for(int i = 0; i < (int)beds.size(); i++) {
				beds[i].bedCharges = newCharge;
			}
			cout << "  [OK] Bed charge updated to " << newCharge << " for all beds." << endl;
			saveToFile();
		} else {
			cout << "  [Error] Invalid choice." << endl;
		}
	}

	// Displays beds filtered by availability status or shows total count
	void get() {
		printHeader("BED VIEW");
		cout << "  0. List available beds" << endl;
		cout << "  1. List reserved beds"  << endl;
		cout << "  2. Show total count"    << endl;
		printLine();
		cout << "  Choice: ";
		int choice = 0;
		cin >> choice;

		printLine();
		switch(choice) {
			case 0:
				for(int i = 0; i < (int)beds.size(); i++) {
					if(!beds[i].reserved) {
						cout << "  Bed " << beds[i].bId << " -- Available" << endl;
					}
				}
				break;
			case 1:
				for(int i = 0; i < (int)beds.size(); i++) {
					if(beds[i].reserved) {
						cout << "  Bed " << beds[i].bId
						     << " -- Reserved by Patient #" << beds[i].pBed << endl;
					}
				}
				break;
			case 2:
				cout << "  Total beds: " << beds.size() << endl;
				break;
			default:
				cout << "  [Error] Invalid choice." << endl;
		}
	}

	// Removes a free (unreserved) bed from the system by its ID
	void removeBed() {
		printLine();
		cout << "  Enter Bed ID to remove: ";
		int id = 0;
		cin >> id;

		int foundIdx = -1;
		for(int i = 0; i < (int)beds.size(); ++i) {
			if(beds[i].bId == id) {
				foundIdx = i;
				break;
			}
		}

		if(foundIdx == -1) {
			cout << "  [Error] Bed ID not found." << endl;
			return;
		}
		if(beds[foundIdx].reserved) {
			cout << "  [Error] Cannot remove reserved bed." << endl;
			return;
		}

		beds.erase(beds.begin() + foundIdx);
		// Re-index remaining beds so IDs stay sequential
		for(int i = 0; i < (int)beds.size(); ++i) {
			beds[i].bId = i;
		}
		cout << "  [OK] Bed removed." << endl;
		saveToFile();
	}

	// Main menu loop for bed management
	void bedMenu() {
		int choice = 0;
		while(choice != 4) {
			printHeader("BED MANAGEMENT");
			cout << "  1. Add / Update Beds" << endl;
			cout << "  2. View Beds"         << endl;
			cout << "  3. Remove a Bed"      << endl;
			cout << "  4. Back"              << endl;
			printLine();
			cout << "  Choice: ";
			cin >> choice;
			switch(choice) {
				case 1: set();       break;
				case 2: get();       break;
				case 3: removeBed(); break;
				case 4:              break;
				default: cout << "  [Error] Invalid choice." << endl;
			}
		}
	}
};


// ============================================================
//  CLASS: MedManagement
//  Owns and manages the pharmacy's medicine inventory.
//  Persists all changes to medicines.txt automatically.
// ============================================================
class MedManagement {
private:
	vector<Medicine> medics; // in-memory list of all medicines

	// Returns the vector index of a medicine by its ID, or -1 if not found
	int findIndexById(int mID) {
		for(int i = 0; i < (int)medics.size(); ++i) {
			if(medics[i].mID == mID) {
				return i;
			}
		}
		return -1;
	}

	// Writes the full medicine list to medicines.txt, overwriting previous content
	void saveToFile() {
		ofstream file("medicines.txt");
		if(!file.is_open()) {
			cout << "  [Error] Could not open medicines.txt for writing." << endl;
			return;
		}
		for(int i = 0; i < (int)medics.size(); i++) {
			file << medics[i].mID          << "|"
			     << medics[i].name         << "|"
			     << medics[i].type         << "|"
			     << medics[i].stock        << "|"
			     << medics[i].pricePerItem << "\n";
		}
		file.close();
	}

public:
	// Constructor — optionally pre-reserves vector capacity
	MedManagement(int reserveCount = 0) {
		if(reserveCount > 0) {
			medics.reserve(reserveCount);
		}
		cout << "  [System] MedManagement initialised." << endl;
	}

	// Loads medicines from medicines.txt on startup.
	// If no file exists, starts with an empty inventory.
	void loadFromFile() {
		ifstream file("medicines.txt");
		if(!file.is_open()) {
			cout << "  [Info] No medicines file found. Starting with empty inventory." << endl;
			return;
		}
		medics.clear();
		string line;
		// Read each line and reconstruct a Medicine from pipe-separated fields
		while(getline(file, line)) {
			if(line.empty()) {
				continue;
			}
			stringstream ss(line);
			string token;
			Medicine m;
			getline(ss, token, '|'); m.mID          = stoi(token);
			getline(ss, token, '|'); m.name         = token;
			getline(ss, token, '|'); m.type         = token;
			getline(ss, token, '|'); m.stock        = stoi(token);
			getline(ss, token, '|'); m.pricePerItem = stod(token);
			medics.push_back(m);
		}
		file.close();
		cout << "  [Info] Medicines loaded: " << medics.size() << endl;
	}

	// Prompts the user to enter one or more medicines into the inventory
	void set() {
		printHeader("ADD MEDICINES");
		char more = 'y';
		while(more == 'y' || more == 'Y') {
			Medicine m;
			m.mID          = (int)medics.size(); // auto-assign next available ID
			m.name         = "";
			m.type         = "";
			m.stock        = 0;
			m.pricePerItem = 0.0;

			cout << "  Enter medicine type (e.g. Tablet, Syrup): ";
			m.type = Validator::getTextOnly();
			cout << "  Enter medicine name: ";
			m.name = Validator::getTextOnly();
			cout << "  Enter price per unit: ";
			m.pricePerItem = Validator::getPositiveDouble();
			cout << "  Enter stock quantity: ";
			m.stock = Validator::getPositiveInt();

			medics.push_back(m);
			saveToFile();
			cout << "  [OK] Medicine added (ID: " << m.mID << ")." << endl;
			printLine();
			cout << "  Add another medicine? (y/n): ";
			cin >> more;
		}
	}

	// Prints all medicines currently in inventory
	void get() {
		printHeader("MEDICINE INVENTORY");
		if(medics.empty()) {
			cout << "  No medicines in inventory." << endl;
			return;
		}
		printLine();
		for(int i = 0; i < (int)medics.size(); i++) {
			const Medicine &m = medics[i];
			cout << "  ID: "    << m.mID
			     << "  Name: "  << m.name
			     << "  Type: "  << m.type
			     << "  Stock: " << m.stock
			     << "  Price: " << m.pricePerItem << endl;
		}
		printLine();
	}

	// Removes a medicine from inventory by its ID
	void removeItem() {
		printLine();
		cout << "  Enter Medicine ID to remove: ";
		int id = 0;
		cin >> id;

		int idx = findIndexById(id);
		if(idx == -1) {
			cout << "  [Error] Medicine ID not found." << endl;
			return;
		}

		medics.erase(medics.begin() + idx);
		// Re-index remaining medicines so IDs stay sequential
		for(int i = 0; i < (int)medics.size(); ++i) {
			medics[i].mID = i;
		}
		cout << "  [OK] Medicine removed." << endl;
		saveToFile();
	}

	// Updates the stock of a medicine.
	// Positive value adds stock, negative value corrects it downward.
	void updateStock() {
		printHeader("UPDATE STOCK");
		cout << "  Enter Medicine ID: ";
		int id = 0;
		cin >> id;

		int idx = findIndexById(id);
		if(idx == -1) {
			cout << "  [Error] Medicine ID not found." << endl;
			return;
		}

		cout << "  Current stock for " << medics[idx].name
		     << ": " << medics[idx].stock << endl;
		cout << "  Enter change (positive to add, negative to correct): ";
		int change = 0;
		cin >> change;

		if(medics[idx].stock + change < 0) {
			cout << "  [Error] Cannot reduce stock below zero." << endl;
			return;
		}
		medics[idx].stock += change;
		cout << "  [OK] New stock for " << medics[idx].name
		     << ": " << medics[idx].stock << endl;
		saveToFile();
	}

	// Deducts qty units of a medicine and fills a Medication record for the patient.
	// Returns false if the ID is invalid or stock is insufficient.
	bool consumeMedicine(int id, int qty, Medication &out) {
		int idx = findIndexById(id);
		if(idx == -1) {
			return false;
		}
		if(qty <= 0 || medics[idx].stock < qty) {
			return false;
		}

		// Deduct stock and take a snapshot of the medicine details
		medics[idx].stock -= qty;
		saveToFile();

		out.medicineId = medics[idx].mID;
		out.name       = medics[idx].name;
		out.type       = medics[idx].type;
		out.quantity   = qty;
		out.price      = medics[idx].pricePerItem;
		return true;
	}

	// Main menu loop for medicine management
	void medicineMenu() {
		int choice = 0;
		while(choice != 5) {
			printHeader("MEDICINE MANAGEMENT");
			cout << "  1. Add Medicines"    << endl;
			cout << "  2. View Inventory"   << endl;
			cout << "  3. Remove Medicine"  << endl;
			cout << "  4. Update Stock"     << endl;
			cout << "  5. Back"             << endl;
			printLine();
			cout << "  Choice: ";
			cin >> choice;
			switch(choice) {
				case 1: set();         break;
				case 2: get();         break;
				case 3: removeItem();  break;
				case 4: updateStock(); break;
				case 5:                break;
				default: cout << "  [Error] Invalid choice." << endl;
			}
		}
	}
};


// ============================================================
//  CLASS: Patient
//  Represents a single admitted patient.
//  Stores identity, admission state, and medication history.
//  Exposes controlled getters for file persistence.
// ============================================================
class Patient {
private:
	static int         nextId;        // shared counter — increments on each new admission
	PatientInfo        info;          // identity fields
	bool               isAdmitted;   // true once patient has been formally admitted
	bool               isDischarged; // true once patient has been discharged
	time_t             admitTime;    // unix timestamp of admission
	time_t             dischargeTime;// unix timestamp of discharge (0 if still admitted)
	struct tm          dateTime;     // human-readable admit date (derived from admitTime)
	double             admitBedCharge; // per-day bed rate locked in at admission
	vector<Medication> meds;         // all medications given during this stay

public:
	// Constructor — initialises a blank patient record
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
	}

	// ---- Getters ----
	int                       id()               const { return info.pID;       }
	long long                 getCnic()           const { return info.cnic;      }
	bool                      getIsDischarged()   const { return isDischarged;   }
	bool                      getIsAdmitted()     const { return isAdmitted;     }
	time_t                    getAdmitTime()      const { return admitTime;      }
	time_t                    getDischargeTime()  const { return dischargeTime;  }
	double                    getAdmitBedCharge() const { return admitBedCharge; }
	const PatientInfo&        getInfo()           const { return info;           }
	const vector<Medication>& getMeds()           const { return meds;           }
	static int                getNextId()               { return nextId;         }

	// ---- Setters (used during file loading to restore private state) ----
	void setCnic(long long value)        { info.cnic    = value; }
	void setName(const string &val)      { info.name    = val;   }
	void setDisease(const string &val)   { info.disease = val;   }
	static void setNextId(int val)       { nextId       = val;   }

	// Restores all private state from saved file values.
	// Used during loading — bypasses normal admission flow intentionally.
	void restoreState(int pid, bool admitted, bool discharged,
	                  time_t aTime, time_t dTime, double charge) {
		info.pID       = pid;
		isAdmitted     = admitted;
		isDischarged   = discharged;
		admitTime      = aTime;
		dischargeTime  = dTime;
		admitBedCharge = charge;
		// Rebuild the human-readable date from the stored timestamp
		if(admitTime != 0) {
			dateTime = *localtime(&admitTime);
			mktime(&dateTime);
		}
	}

	// Adds a medication directly to the patient's list without user interaction.
	// Used during file loading to restore medication history.
	void addMedicationDirect(const Medication &med) {
		meds.push_back(med);
	}

	// Formally admits the patient — records timestamp, increments global ID counter
	void markAdmitted(double bedCharge) {
		if(!isAdmitted) {
			admitTime      = time(nullptr);
			dateTime       = *localtime(&admitTime);
			mktime(&dateTime);
			isAdmitted     = true;
			admitBedCharge = bedCharge;
			info.pID       = ++nextId;
			cout << "  [OK] Patient admitted. ID: " << info.pID << endl;
		}
	}

	// Formally discharges the patient — records discharge timestamp
	void markDischarged() {
		if(isAdmitted && !isDischarged) {
			dischargeTime = time(nullptr);
			isDischarged  = true;
			cout << "  [OK] Discharge time recorded." << endl;
		}
	}

	// Prompts the user to enter the patient's name and disease
	void set() {
		cout << "  Enter patient name: ";
		info.name = Validator::getTextOnly();
		if(info.cnic == 0) {
			cout << "  Enter patient CNIC: ";
			cin >> info.cnic;
		}
		cout << "  Enter diagnosed disease: ";
		info.disease = Validator::getDiagnosis();
	}

	// Displays the patient's full record to the console
	void patientHistory() {
		printHeader("PATIENT RECORD");
		cout << "  Patient ID    : " << info.pID        << endl;
		cout << "  Name          : " << info.name       << endl;
		cout << "  CNIC          : " << info.cnic       << endl;
		cout << "  Disease       : " << info.disease    << endl;
		cout << "  Bed Charge/Day: " << admitBedCharge  << endl;
		cout << "  Admit Date    : " << asctime(&dateTime);
		if(isDischarged) {
			struct tm dischargeTm = *localtime(&dischargeTime);
			cout << "  Discharge Date: " << asctime(&dischargeTm);
		} else {
			cout << "  Discharge Date: Still admitted" << endl;
		}
		printLine();
	}

	// Prompts user to select a medicine and quantity, deducts from inventory,
	// and adds it to this patient's medication list
	void addMedication(MedManagement &medManager) {
		if(isDischarged) {
			cout << "  [Error] Cannot add medication — patient already discharged." << endl;
			return;
		}
		cout << "  Enter Medicine ID: ";
		int medId = 0;
		cin >> medId;
		cout << "  Enter quantity: ";
		int qty = 0;
		cin >> qty;

		Medication med;
		if(medManager.consumeMedicine(medId, qty, med)) {
			meds.push_back(med);
			cout << "  [OK] Medication added." << endl;
		} else {
			cout << "  [Error] Invalid medicine ID or insufficient stock." << endl;
		}
	}

	// Prints all medications given to this patient during their stay
	void displayMedication() {
		printHeader("MEDICATIONS");
		if(meds.empty()) {
			cout << "  No medications recorded." << endl;
			return;
		}
		printLine();
		for(int i = 0; i < (int)meds.size(); i++) {
			const Medication &m = meds[i];
			cout << "  " << m.name << " (" << m.type << ")"
			     << "  Qty: " << m.quantity
			     << "  Unit Price: " << m.price << endl;
		}
		printLine();
	}

	// Calculates and displays the full bill for this patient.
	// Uses actual discharge time if discharged, otherwise uses current time.
	void generateBill() {
		if(!isAdmitted) {
			cout << "  [Error] Patient has no admission record." << endl;
			return;
		}

		time_t endTime = (dischargeTime != 0) ? dischargeTime : time(nullptr);
		int days = (int)(difftime(endTime, admitTime) / 86400);
		if(days < 1) {
			days = 1; // minimum one day charge
		}

		// Sum all medication costs
		double medCharges = 0.0;
		for(int i = 0; i < (int)meds.size(); i++) {
			medCharges += meds[i].price * (double)meds[i].quantity;
		}

		double bedCharges = admitBedCharge * (double)days;
		double total      = medCharges + bedCharges;

		printHeader("BILL SUMMARY");
		cout << "  Days Stayed         : " << days        << endl;
		printLine();
		cout << "  Medication Charges  : Rs. " << medCharges << endl;
		cout << "  Bed Charges         : Rs. " << bedCharges << endl;
		printLine();
		cout << "  TOTAL               : Rs. " << total      << endl;
		printLine();
	}
};

// Static member definition — exists once, shared across all Patient instances
int Patient::nextId = 0;


// ============================================================
//  CLASS: PatientManagement
//  Manages the full list of patients in the hospital.
//  Coordinates with BedManagement and MedManagement.
//  Persists patient and medication data to separate files.
// ============================================================
class PatientManagement {
private:
	vector<Patient>  patients;   // in-memory list of all patients
	BedManagement   &bedManager; // reference to shared bed system
	MedManagement   &medManager; // reference to shared medicine system

	// Writes all patient records to patients.txt.
	// First line is nextId so it can be restored on next startup.
	void savePatientsToFile() {
		ofstream file("patients.txt");
		if(!file.is_open()) {
			cout << "  [Error] Could not open patients.txt for writing." << endl;
			return;
		}
		file << Patient::getNextId() << "\n"; // save ID counter on first line
		for(int i = 0; i < (int)patients.size(); i++) {
			const Patient     &p   = patients[i];
			const PatientInfo &inf = p.getInfo();
			file << inf.pID               << "|"
			     << inf.name              << "|"
			     << inf.cnic              << "|"
			     << inf.disease           << "|"
			     << p.getIsAdmitted()     << "|"
			     << p.getIsDischarged()   << "|"
			     << p.getAdmitTime()      << "|"
			     << p.getDischargeTime()  << "|"
			     << p.getAdmitBedCharge() << "\n";
		}
		file.close();
	}

	// Writes all medication records to medications.txt.
	// Each line is prefixed with the patient ID (foreign key).
	void saveMedicationsToFile() {
		ofstream file("medications.txt");
		if(!file.is_open()) {
			cout << "  [Error] Could not open medications.txt for writing." << endl;
			return;
		}
		for(int i = 0; i < (int)patients.size(); i++) {
			const Patient            &p    = patients[i];
			const vector<Medication> &meds = p.getMeds();
			for(int j = 0; j < (int)meds.size(); j++) {
				file << p.id()             << "|"
				     << meds[j].medicineId << "|"
				     << meds[j].name       << "|"
				     << meds[j].type       << "|"
				     << meds[j].quantity   << "|"
				     << meds[j].price      << "\n";
			}
		}
		file.close();
	}

	// Returns the vector index of a patient by CNIC, or -1 if not found
	int findIndexByCnic(long long cnic) {
		for(int i = 0; i < (int)patients.size(); ++i) {
			if(patients[i].getCnic() == cnic) {
				return i;
			}
		}
		return -1;
	}

	// Returns a pointer to a patient by CNIC, or nullptr if not found
	Patient* findByCnic(long long cnic) {
		for(int i = 0; i < (int)patients.size(); ++i) {
			if(patients[i].getCnic() == cnic) {
				return &patients[i];
			}
		}
		return nullptr;
	}

public:
	// Constructor — takes references to shared managers
	PatientManagement(BedManagement &b, MedManagement &m)
		: bedManager(b), medManager(m) {
		cout << "  [System] PatientManagement initialised." << endl;
	}

	// Loads patient records from patients.txt on startup.
	// First line restores the ID counter, remaining lines restore patient rows.
	void loadPatientsFromFile() {
		ifstream file("patients.txt");
		if(!file.is_open()) {
			cout << "  [Info] No patients file found. Starting fresh." << endl;
			return;
		}
		patients.clear();
		string line;

		// First line — restore the global patient ID counter
		if(getline(file, line) && !line.empty()) {
			Patient::setNextId(stoi(line));
		}

		// Remaining lines — restore each patient record
		while(getline(file, line)) {
			if(line.empty()) {
				continue;
			}
			stringstream ss(line);
			string token;
			Patient p;

			getline(ss, token, '|'); int    pid        = stoi(token);
			getline(ss, token, '|'); p.setName(token);
			getline(ss, token, '|'); p.setCnic(stoll(token));
			getline(ss, token, '|'); p.setDisease(token);
			getline(ss, token, '|'); bool   admitted   = stoi(token);
			getline(ss, token, '|'); bool   discharged = stoi(token);
			getline(ss, token, '|'); time_t aTime      = (time_t)stoll(token);
			getline(ss, token, '|'); time_t dTime      = (time_t)stoll(token);
			getline(ss, token, '|'); double charge     = stod(token);

			p.restoreState(pid, admitted, discharged, aTime, dTime, charge);
			patients.push_back(p);
		}
		file.close();
		cout << "  [Info] Patients loaded: " << patients.size() << endl;
	}

	// Loads medication records from medications.txt on startup.
	// Each row uses the patient ID (foreign key) to find the correct patient.
	// Must be called AFTER loadPatientsFromFile().
	void loadMedicationFromFile() {
		ifstream file("medications.txt");
		if(!file.is_open()) {
			cout << "  [Info] No medications file found. Starting fresh." << endl;
			return;
		}
		string line;
		while(getline(file, line)) {
			if(line.empty()) {
				continue;
			}
			stringstream ss(line);
			string token;
			Medication med;
			int pid = 0;

			getline(ss, token, '|'); pid            = stoi(token);
			getline(ss, token, '|'); med.medicineId = stoi(token);
			getline(ss, token, '|'); med.name       = token;
			getline(ss, token, '|'); med.type       = token;
			getline(ss, token, '|'); med.quantity   = stoi(token);
			getline(ss, token, '|'); med.price      = stod(token);

			// Find the patient this medication belongs to and attach it
			for(int i = 0; i < (int)patients.size(); i++) {
				if(patients[i].id() == pid) {
					patients[i].addMedicationDirect(med);
					break;
				}
			}
		}
		file.close();
		cout << "  [Info] Medications loaded." << endl;
	}

	// Admits a new patient — checks bed availability first, then collects info
	void addPatient() {
		printHeader("NEW PATIENT ADMISSION");
		cout << "  Enter patient CNIC: ";
		long long cnic = Validator::getCnic();

		if(findIndexByCnic(cnic) != -1) {
			cout << "  [Error] A patient with this CNIC already exists." << endl;
			return;
		}

		// Step 1 — confirm a bed is available before collecting any info
		double assignedCharge = 0.0;
		int bedIdx = bedManager.findFreeBed(assignedCharge);
		if(bedIdx == -1) {
			cout << "  [Error] No beds available. Cannot admit patient." << endl;
			return;
		}

		// Step 2 — collect patient details and formally admit (assigns real ID)
		Patient p;
		p.setCnic(cnic);
		p.set();
		p.markAdmitted(assignedCharge);

		// Step 3 — assign the confirmed bed to the patient's real ID
		bedManager.assignBedByIndex(bedIdx, p.id());

		patients.push_back(p);
		savePatientsToFile();
		cout << "  [OK] Patient admitted successfully." << endl;
	}

	// Discharges a patient — releases their bed and records discharge time
	void dischargePatient() {
		printHeader("PATIENT DISCHARGE");
		cout << "  Enter patient CNIC: ";
		long long cnic = Validator::getCnic();

		Patient *p = findByCnic(cnic);
		if(!p) {
			cout << "  [Error] Patient not found." << endl;
			return;
		}
		if(p->getIsDischarged()) {
			cout << "  [Error] Patient already discharged." << endl;
			return;
		}

		bedManager.resignBed(p->id()); // free the bed
		p->markDischarged();           // record discharge timestamp
		savePatientsToFile();

		cout << endl;
		p->generateBill();
		cout << "  [OK] Patient discharged. Record retained for reference." << endl;
	}

	// Permanently removes a patient record from the system.
	// Warns if the patient has not been discharged yet.
	void removePatient() {
		printHeader("REMOVE PATIENT RECORD");
		cout << "  Enter patient CNIC: ";
		long long cnic = Validator::getCnic();

		int idx = findIndexByCnic(cnic);
		if(idx == -1) {
			cout << "  [Error] Patient not found." << endl;
			return;
		}

		if(!patients[idx].getIsDischarged()) {
			cout << "  [Warning] Patient is still admitted. Force remove? (y/n): ";
			char c;
			cin >> c;
			if(c != 'y' && c != 'Y') {
				return;
			}
			bedManager.resignBed(patients[idx].id());
		}

		patients.erase(patients.begin() + idx);
		savePatientsToFile();
		saveMedicationsToFile();
		cout << "  [OK] Patient record removed." << endl;
	}

	// Opens a submenu to manage an existing patient's record
	void managePatient() {
		printHeader("MANAGE PATIENT");
		cout << "  Enter patient CNIC: ";
		long long cnic = Validator::getCnic();

		if(!findByCnic(cnic)) {
			cout << "  [Error] Patient not found." << endl;
			return;
		}

		int choice = 0;
		while(choice != 5) {
			// Re-fetch pointer each iteration in case vector reallocated
			Patient *p = findByCnic(cnic);
			if(!p) {
				cout << "  [Error] Patient record no longer exists." << endl;
				return;
			}

			printHeader("PATIENT OPTIONS");
			cout << "  1. View Patient History"  << endl;
			cout << "  2. Add Medication"        << endl;
			cout << "  3. View Medications"      << endl;
			cout << "  4. Generate Bill"         << endl;
			cout << "  5. Back"                  << endl;
			printLine();
			cout << "  Choice: ";
			cin >> choice;

			switch(choice) {
				case 1: p->patientHistory(); break;
				case 2:
					p->addMedication(medManager);
					saveMedicationsToFile();
					break;
				case 3: p->displayMedication(); break;
				case 4: p->generateBill();      break;
				case 5:                         break;
				default: cout << "  [Error] Invalid choice." << endl;
			}
		}
	}

	// Main menu loop for patient management
	void patientManagementMenu() {
		int choice = 0;
		while(choice != 5) {
			printHeader("PATIENT MANAGEMENT");
			cout << "  1. Admit New Patient"       << endl;
			cout << "  2. Discharge Patient"       << endl;
			cout << "  3. Remove Patient Record"   << endl;
			cout << "  4. Manage Existing Patient" << endl;
			cout << "  5. Back"                    << endl;
			printLine();
			cout << "  Choice: ";
			cin >> choice;
			switch(choice) {
				case 1: addPatient();       break;
				case 2: dischargePatient(); break;
				case 3: removePatient();    break;
				case 4: managePatient();    break;
				case 5:                     break;
				default: cout << "  [Error] Invalid choice." << endl;
			}
		}
	}
};


// ============================================================
//  STRUCT: UserLogin
//  Holds admin credentials for system access
// ============================================================
struct UserLogin {
	int    uID; // numeric user ID
	string pwd; // password
};


// ============================================================
//  CLASS: HMS  (Hospital Management System)
//  Root controller — owns all subsystems.
//  Handles login and routes the main menu.
// ============================================================
class HMS {
private:
	UserLogin         user;
	BedManagement     bedManager;
	MedManagement     medManager;
	PatientManagement patientManager;

	// Reads a password from the keyboard, masking each character with '*'.
	// Handles backspace for corrections.
	string getPassword() {
		string pwd = "";
		int ch;
		cout << "  (Characters hidden as * while typing)" << endl;
		cout << "  Password: ";
		while(true) {
			ch = _getch();
			if(ch == '\r' || ch == '\n') {
				break; // Enter — done
			} else if(ch == 0 || ch == 224) {
				_getch(); // arrow keys — ignore
			} else if(ch == 8 || ch == 127) { // backspace — delete last char
				if(!pwd.empty()) {
					pwd.pop_back();
					cout << "\b \b" << flush;
				}
			} else if(ch >= 32 && ch < 127) { // printable character
				pwd += ch;
				cout << "*" << flush;
			}
		}
		cout << endl;
		return pwd;
	}

public:
	// Constructor — sets credentials then loads all persisted data from disk
	HMS(int defaultUserId = 4040, string defaultPassword = "Admin")
		: patientManager(bedManager, medManager)
	{
		user.uID = defaultUserId;
		user.pwd = defaultPassword;
		cout << "  [System] HMS initialised." << endl;

		// Load order matters — beds and medicines must exist before patients
		bedManager.loadFromFile();
		medManager.loadFromFile();
		patientManager.loadPatientsFromFile();
		patientManager.loadMedicationFromFile();
	}

	// Presents the login screen and validates credentials.
	// Allows up to 3 attempts before locking the session.
	bool Login() {
		int    tries     = 3;
		int    uInputId  = 0;
		string uInputPwd = "";

		printHeader("HMS LOGIN");
		cout << "  Please enter your ID and password to continue." << endl;
		printLine();

		while(tries > 0) {
			cout << "  Attempts remaining: " << tries << endl;
			cout << "  Enter ID: ";
			cin >> uInputId;
			uInputPwd = getPassword();

			if(uInputId == user.uID && uInputPwd == user.pwd) {
				cout << "  [OK] Login successful. Welcome." << endl;
				return true;
			}
			--tries;
			if(tries > 0) {
				cout << "  [Error] Incorrect credentials. Try again." << endl;
			} else {
				cout << "  [Error] Incorrect credentials." << endl;
			}
		}
		cout << "  [Locked] Too many failed attempts. Session terminated." << endl;
		return false;
	}

	// Main application loop — shown after successful login
	void menu() {
		if(!Login()) {
			return;
		}

		int choice = 0;
		while(choice != 4) {
			printHeader("HOSPITAL MANAGEMENT SYSTEM");
			cout << "  1. Patient Management"  << endl;
			cout << "  2. Medicine Management" << endl;
			cout << "  3. Bed Management"      << endl;
			cout << "  4. Exit"                << endl;
			printLine();
			cout << "  Choice: ";
			cin >> choice;

			switch(choice) {
				case 1: patientManager.patientManagementMenu(); break;
				case 2: medManager.medicineMenu();              break;
				case 3: bedManager.bedMenu();                   break;
				case 4:
					printLine();
					cout << "  Goodbye. Stay healthy." << endl;
					printLine();
					break;
				default: cout << "  [Error] Invalid choice." << endl;
			}
		}
	}
};


// ============================================================
//  ENTRY POINT
// ============================================================
int main() {
	HMS obj;
	obj.menu();
	return 0;
}
