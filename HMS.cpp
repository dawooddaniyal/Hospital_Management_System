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

		// Returns a valid 11-digit numeric phone number
		static long long getPhone() {
			string input;
			while(true) {
				getline(cin >> ws, input);
				if(input.size() != 11) {
					cout << "  [Error] Phone must be exactly 11 digits. Try again: ";
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
					cout << "  [Error] Phone must contain digits only. Try again: ";
					continue;
				}
				return stoll(input);
			}
		}
};


// ============================================================
//  CLASS: Person
//  Abstract base class for all people in the hospital system.
//  Cannot be instantiated directly — only Doctor and Patient
//  can be created, both of which must implement set() and display().
// ============================================================
class Person {
protected:
	string    name;   // full name
	long long cnic;   // national identity number (13 digits)
	int       age;    // age in years
	string    gender; // Male or Female
	long long phone;  // 11-digit phone number

	// Shared input helper — collects all base fields.
	// Called by derived classes inside their own set().
	// Not virtual — just a reusable utility for subclasses.
	void collectBasicInfo() {
		cout << "  Enter name: ";
		name = Validator::getTextOnly();
		cout << "  Enter CNIC (13 digits): ";
		cnic = Validator::getCnic();
		cout << "  Enter age: ";
		age = Validator::getPositiveInt();
		cout << "  Enter gender (Male/Female): ";
		gender = Validator::getTextOnly();
		cout << "  Enter phone number (11 digits): ";
		phone = Validator::getPhone();
	}

public:
	// Constructor — safe zero/empty defaults for all fields
	Person() : name(""), cnic(0), age(0), gender(""), phone(0) {}

	// ---- Getters ----
	string    getName()   const { return name;   }
	long long getCnic()   const { return cnic;   }
	int       getAge()    const { return age;    }
	string    getGender() const { return gender; }
	long long getPhone()  const { return phone;  }

	// ---- Setters — used only during file loading to restore state ----
	void setName(const string &val)   { name   = val; }
	void setCnic(long long val)        { cnic   = val; }
	void setAge(int val)               { age    = val; }
	void setGender(const string &val) { gender = val; }
	void setPhone(long long val)       { phone  = val; }

	// Pure virtual — every derived class MUST implement these two methods.
	// Leaving either unimplemented makes the derived class abstract too.
	virtual void set()     = 0;
	virtual void display() = 0;

	// Virtual destructor — mandatory whenever a class is used as a base.
	// Ensures derived class destructor is called correctly via base pointer.
	virtual ~Person() {}
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
// PatientInfo has been dissolved — Patient now inherits identity from Person directly.
struct Medication {
	int    medicineId; // ID of the source medicine
	string name;       // name at time of prescription
	string type;       // type at time of prescription
	int    quantity;   // number of units prescribed
	double price;      // unit price at time of prescription
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
//  DISEASE LIST
//  Fixed menu of diseases mapped to hospital specialties.
//  Used during patient admission to replace free-text diagnosis.
//  Patient picks a number — system records both disease and specialty.
// ============================================================
struct DiseaseEntry {
	string diseaseName; // what gets stored on the patient record
	string specialty;   // which doctor specialty should visit this patient
};

const int DISEASE_COUNT = 10;

DiseaseEntry diseaseList[DISEASE_COUNT] = {
	{ "Flu",           "General"     },  //  1
	{ "Fever",         "General"     },  //  2
	{ "Typhoid",       "General"     },  //  3
	{ "Hypertension",  "Cardiology"  },  //  4
	{ "Heart Disease", "Cardiology"  },  //  5
	{ "Epilepsy",      "Neurology"   },  //  6
	{ "Migraine",      "Neurology"   },  //  7
	{ "Fracture",      "Orthopedics" },  //  8
	{ "Joint Pain",    "Orthopedics" },  //  9
	{ "Asthma",        "Pulmonology" }   // 10
};


// ============================================================
//  CLASS: Patient
//  Represents a single admitted patient.
//  Stores identity, admission state, and medication history.
//  Exposes controlled getters for file persistence.
// ============================================================
class Patient : public Person {
private:
	static int         nextId;        // shared counter — increments on each new admission
	int                pID;           // unique patient identifier (was info.pID)
	string             disease;       // diagnosed condition (from disease menu)
	string             requiredSpecialty; // specialty needed — derived from disease choice
	bool               isAdmitted;   // true once patient has been formally admitted
	bool               isDischarged; // true once patient has been discharged
	time_t             admitTime;    // unix timestamp of admission
	time_t             dischargeTime;// unix timestamp of discharge (0 if still admitted)
	struct tm          dateTime;     // human-readable admit date (derived from admitTime)
	double             admitBedCharge; // per-day bed rate locked in at admission
	vector<Medication> meds;         // all medications given during this stay

	// Private helper — shows the numbered disease menu and sets disease + specialty.
	// Called inside set() after collectBasicInfo().
	void showDiseaseMenu() {
		printHeader("SELECT DISEASE");
		for(int i = 0; i < DISEASE_COUNT; i++) {
			cout << "  " << (i + 1) << ". "
			     << diseaseList[i].diseaseName
			     << "  [" << diseaseList[i].specialty << "]" << endl;
		}
		printLine();
		int choice = 0;
		cout << "  Enter number (1-" << DISEASE_COUNT << "): ";
		while(true) {
			cin >> choice;
			if(choice >= 1 && choice <= DISEASE_COUNT) {
				break;
			}
			cout << "  [Error] Enter a number between 1 and "
			     << DISEASE_COUNT << ": ";
		}
		disease           = diseaseList[choice - 1].diseaseName;
		requiredSpecialty = diseaseList[choice - 1].specialty;
		cout << "  [OK] Disease set to: " << disease
		     << "  |  Specialty: " << requiredSpecialty << endl;
	}

public:
	// Constructor — initialises a blank patient record
	Patient() {
		pID               = 0;
		disease           = "";
		requiredSpecialty = "";
		isAdmitted        = false;
		isDischarged      = false;
		admitTime         = 0;
		dischargeTime     = 0;
		admitBedCharge    = 0.0;
		memset(&dateTime, 0, sizeof(dateTime));
	}

	// ---- Getters ----
	int                       id()                   const { return pID;               }
	bool                      getIsDischarged()       const { return isDischarged;      }
	bool                      getIsAdmitted()         const { return isAdmitted;        }
	time_t                    getAdmitTime()          const { return admitTime;         }
	time_t                    getDischargeTime()      const { return dischargeTime;     }
	double                    getAdmitBedCharge()     const { return admitBedCharge;    }
	string                    getDisease()            const { return disease;           }
	string                    getRequiredSpecialty()  const { return requiredSpecialty; }
	const vector<Medication>& getMeds()               const { return meds;             }
	static int                getNextId()                   { return nextId;           }

	// ---- Setters — used only during file loading to restore state ----
	// name, cnic, age, gender, phone setters are inherited from Person
	void setDisease(const string &val)           { disease           = val; }
	void setRequiredSpecialty(const string &val) { requiredSpecialty = val; }
	static void setNextId(int val)               { nextId            = val; }

	// Restores all private state from saved file values.
	// Used during loading — bypasses normal admission flow intentionally.
	void restoreState(int pid, bool admitted, bool discharged,
	                  time_t aTime, time_t dTime, double charge) {
		pID            = pid;
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
			pID            = ++nextId;
			cout << "  [OK] Patient admitted. ID: " << pID << endl;
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

	// Implements Person::set() — collects personal info then disease from menu
	void set() override {
		collectBasicInfo();   // name, cnic, age, gender, phone via Person
		showDiseaseMenu();    // disease and requiredSpecialty via menu
	}

	// Implements Person::display() — shows full identity and medical snapshot
	void display() override {
		cout << "  Patient ID        : " << pID               << endl;
		cout << "  Name              : " << name              << endl;
		cout << "  CNIC              : " << cnic              << endl;
		cout << "  Age               : " << age               << endl;
		cout << "  Gender            : " << gender            << endl;
		cout << "  Phone             : " << phone             << endl;
		cout << "  Disease           : " << disease           << endl;
		cout << "  Required Specialty: " << requiredSpecialty << endl;
	}

	// Displays the patient's full record including admission dates and bed charge.
	// Calls display() first for identity/medical info, then adds admission context.
	void patientHistory() {
		printHeader("PATIENT RECORD");
		display();
		printLine();
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
//  SPECIALTY LIST
//  Fixed menu of doctor specialties — mirrors the disease list.
//  Guarantees every specialty a doctor can hold matches a disease
//  category, enabling correct doctor-to-patient routing.
// ============================================================
struct SpecialtyEntry {
	string specialtyName; // displayed in menu and stored on Doctor record
};

const int SPECIALTY_COUNT = 5;

SpecialtyEntry specialtyList[SPECIALTY_COUNT] = {
	{ "General"      },  // 1
	{ "Cardiology"   },  // 2
	{ "Neurology"    },  // 3
	{ "Orthopedics"  },  // 4
	{ "Pulmonology"  }   // 5
};


// ============================================================
//  CLASS: Doctor
//  Represents a single doctor in the hospital.
//  Inherits identity fields from Person.
//  Adds specialty, shift hours, and duty-check logic.
// ============================================================
class Doctor : public Person {
private:
	static int nextId;  // shared counter — separate from Patient::nextId
	int        dID;     // unique doctor identifier
	string     specialty;   // matched against patient requiredSpecialty
	int        shiftStart;  // shift start hour (9–17), 24-hour format
	int        shiftEnd;    // shift end hour (10–19), 24-hour format

	// Private helper — shows the numbered specialty menu and sets specialty.
	// Called inside set() after collectBasicInfo().
	void showSpecialtyMenu() {
		printHeader("SELECT SPECIALTY");
		for(int i = 0; i < SPECIALTY_COUNT; i++) {
			cout << "  " << (i + 1) << ". "
			     << specialtyList[i].specialtyName << endl;
		}
		printLine();
		int choice = 0;
		cout << "  Enter number (1-" << SPECIALTY_COUNT << "): ";
		while(true) {
			cin >> choice;
			if(choice >= 1 && choice <= SPECIALTY_COUNT) {
				break;
			}
			cout << "  [Error] Enter a number between 1 and "
			     << SPECIALTY_COUNT << ": ";
		}
		specialty = specialtyList[choice - 1].specialtyName;
		cout << "  [OK] Specialty set to: " << specialty << endl;
	}

	// Private helper — prompts and validates shift start and end hours.
	// Start: 9–17   End: 10–19   End must be greater than Start.
	// Think of it as booking a slot on a 9AM–7PM hospital clock.
	void inputShiftHours() {
		printLine();
		cout << "  Enter shift start hour (9-17, e.g. 9 for 9AM): ";
		while(true) {
			cin >> shiftStart;
			if(shiftStart >= 9 && shiftStart <= 17) {
				break;
			}
			cout << "  [Error] Start hour must be between 9 and 17: ";
		}
		cout << "  Enter shift end hour (10-19, e.g. 17 for 5PM): ";
		while(true) {
			cin >> shiftEnd;
			if(shiftEnd >= 10 && shiftEnd <= 19 && shiftEnd > shiftStart) {
				break;
			}
			cout << "  [Error] End hour must be between 10 and 19, and after start hour: ";
		}
	}

public:
	// Constructor — initialises a blank doctor record
	Doctor() {
		dID        = 0;
		specialty  = "";
		shiftStart = 9;
		shiftEnd   = 17;
	}

	// ---- Getters ----
	int    getDID()        const { return dID;        }
	string getSpecialty()  const { return specialty;  }
	int    getShiftStart() const { return shiftStart; }
	int    getShiftEnd()   const { return shiftEnd;   }
	static int getNextId()           { return nextId;   }
	static int incrementNextId()     { return ++nextId; } // used by addDoctor() after duplicate check

	// ---- Setters — used only during file loading to restore state ----
	// name, cnic, age, gender, phone setters inherited from Person
	void setDID(int val)               { dID        = val; }
	void setSpecialty(const string &v) { specialty  = v;   }
	void setShiftStart(int val)        { shiftStart = val; }
	void setShiftEnd(int val)          { shiftEnd   = val; }
	static void setNextId(int val)     { nextId     = val; }

	// Returns true if the given hour falls within this doctor's shift.
	// Used by SlotManagement to enforce duty-hour boundaries.
	// Think of it as checking whether a store is open at a given time.
	bool isOnDutyAt(int hour) const {
		return (hour >= shiftStart && hour < shiftEnd);
	}

	// Implements Person::set() — collects personal info, specialty, shift
	void set() override {
		collectBasicInfo();    // name, cnic, age, gender, phone via Person
		showSpecialtyMenu();   // specialty via fixed menu
		inputShiftHours();     // shift start and end hours
		// ID is NOT assigned here — DoctorManagement::addDoctor() assigns
		// it after the duplicate CNIC check passes, same pattern as Patient.
	}

	// Implements Person::display() — shows full doctor profile
	void display() override {
		cout << "  Doctor ID   : " << dID                          << endl;
		cout << "  Name        : " << name                         << endl;
		cout << "  CNIC        : " << cnic                         << endl;
		cout << "  Age         : " << age                          << endl;
		cout << "  Gender      : " << gender                       << endl;
		cout << "  Phone       : " << phone                        << endl;
		cout << "  Specialty   : " << specialty                    << endl;
		cout << "  Shift       : " << shiftStart << ":00 — "
		                           << shiftEnd   << ":00"          << endl;
	}
};

// Static member definition — exists once, separate from Patient::nextId
int Doctor::nextId = 0;


// ============================================================
//  CLASS: DoctorManagement
//  Owns and manages the hospital's doctor roster.
//  Persists all changes to doctors.txt automatically.
//  Provides specialty-based lookup for SlotManagement.
// ============================================================
class DoctorManagement {
private:
	vector<Doctor> doctors; // in-memory list of all doctors

	// Writes the full doctor list to doctors.txt, overwriting previous content.
	// First line is nextId so the ID counter survives restarts.
	void saveToFile() {
		ofstream file("doctors.txt");
		if(!file.is_open()) {
			cout << "  [Error] Could not open doctors.txt for writing." << endl;
			return;
		}
		file << Doctor::getNextId() << "\n"; // save ID counter on first line
		for(int i = 0; i < (int)doctors.size(); i++) {
			const Doctor &d = doctors[i];
			file << d.getDID()        << "|"
			     << d.getName()       << "|"
			     << d.getCnic()       << "|"
			     << d.getAge()        << "|"
			     << d.getGender()     << "|"
			     << d.getPhone()      << "|"
			     << d.getSpecialty()  << "|"
			     << d.getShiftStart() << "|"
			     << d.getShiftEnd()   << "\n";
		}
		file.close();
	}

	// Returns the vector index of a doctor by their ID, or -1 if not found
	int findIndexById(int id) {
		for(int i = 0; i < (int)doctors.size(); ++i) {
			if(doctors[i].getDID() == id) {
				return i;
			}
		}
		return -1;
	}

public:
	// Constructor
	DoctorManagement() {
		cout << "  [System] DoctorManagement initialised." << endl;
	}

	// Loads doctors from doctors.txt on startup.
	// If no file exists, starts with an empty roster.
	void loadFromFile() {
		ifstream file("doctors.txt");
		if(!file.is_open()) {
			cout << "  [Info] No doctors file found. Starting with empty roster." << endl;
			return;
		}
		doctors.clear();
		string line;

		// First line — restore the global doctor ID counter
		if(getline(file, line) && !line.empty()) {
			Doctor::setNextId(stoi(line));
		}

		// Remaining lines — restore each doctor record
		while(getline(file, line)) {
			if(line.empty()) {
				continue;
			}
			stringstream ss(line);
			string token;
			Doctor d;

			getline(ss, token, '|'); d.setDID(stoi(token));
			getline(ss, token, '|'); d.setName(token);
			getline(ss, token, '|'); d.setCnic(stoll(token));
			getline(ss, token, '|'); d.setAge(stoi(token));
			getline(ss, token, '|'); d.setGender(token);
			getline(ss, token, '|'); d.setPhone(stoll(token));
			getline(ss, token, '|'); d.setSpecialty(token);
			getline(ss, token, '|'); d.setShiftStart(stoi(token));
			getline(ss, token, '|'); d.setShiftEnd(stoi(token));

			doctors.push_back(d);
		}
		file.close();
		cout << "  [Info] Doctors loaded: " << doctors.size() << endl;
	}

	// Prompts the user to register a new doctor into the system
	void addDoctor() {
		printHeader("REGISTER NEW DOCTOR");
		Doctor d;
		d.set();

		// Duplicate CNIC check — run before assigning ID to avoid wasting counter
		for(int i = 0; i < (int)doctors.size(); ++i) {
			if(doctors[i].getCnic() == d.getCnic()) {
				cout << "  [Error] A doctor with this CNIC already exists." << endl;
				return;
			}
		}

		// Assign ID only after duplicate check passes — same pattern as Patient
		d.setDID(Doctor::incrementNextId());
		doctors.push_back(d);
		saveToFile();
		cout << "  [OK] Doctor registered. ID: " << d.getDID() << endl;
	}

	// Removes a doctor from the roster by their ID
	void removeDoctor() {
		printHeader("REMOVE DOCTOR");
		cout << "  Enter Doctor ID to remove: ";
		int id = 0;
		cin >> id;

		int idx = findIndexById(id);
		if(idx == -1) {
			cout << "  [Error] Doctor ID not found." << endl;
			return;
		}

		doctors.erase(doctors.begin() + idx);
		cout << "  [OK] Doctor removed." << endl;
		saveToFile();
	}

	// Displays all doctors currently in the roster
	void viewAllDoctors() {
		printHeader("DOCTOR ROSTER");
		if(doctors.empty()) {
			cout << "  No doctors registered." << endl;
			return;
		}
		for(int i = 0; i < (int)doctors.size(); i++) {
			printLine();
			doctors[i].display();
		}
		printLine();
	}

	// Returns a pointer to a doctor by their ID, or nullptr if not found.
	// Used by SlotManagement to validate and access doctor details.
	Doctor* getDoctorById(int id) {
		for(int i = 0; i < (int)doctors.size(); ++i) {
			if(doctors[i].getDID() == id) {
				return &doctors[i];
			}
		}
		return nullptr;
	}

	// Returns a vector of pointers to all doctors with a matching specialty.
	// Used by SlotManagement to find eligible doctors for a patient's disease.
	vector<Doctor*> getDoctorsBySpecialty(const string &spec) {
		vector<Doctor*> result;
		for(int i = 0; i < (int)doctors.size(); ++i) {
			if(doctors[i].getSpecialty() == spec) {
				result.push_back(&doctors[i]);
			}
		}
		return result;
	}

	// Main menu loop for doctor management
	void doctorMenu() {
		int choice = 0;
		while(choice != 4) {
			printHeader("DOCTOR MANAGEMENT");
			cout << "  1. Register New Doctor" << endl;
			cout << "  2. View All Doctors"    << endl;
			cout << "  3. Remove Doctor"       << endl;
			cout << "  4. Back"                << endl;
			printLine();
			cout << "  Choice: ";
			cin >> choice;
			switch(choice) {
				case 1: addDoctor();     break;
				case 2: viewAllDoctors(); break;
				case 3: removeDoctor();  break;
				case 4:                  break;
				default: cout << "  [Error] Invalid choice." << endl;
			}
		}
	}
};


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
			const Patient &p = patients[i];
			file << p.id()                   << "|"
			     << p.getName()              << "|"
			     << p.getCnic()              << "|"
			     << p.getAge()               << "|"
			     << p.getGender()            << "|"
			     << p.getPhone()             << "|"
			     << p.getDisease()           << "|"
			     << p.getRequiredSpecialty() << "|"
			     << p.getIsAdmitted()        << "|"
			     << p.getIsDischarged()      << "|"
			     << p.getAdmitTime()         << "|"
			     << p.getDischargeTime()     << "|"
			     << p.getAdmitBedCharge()    << "\n";
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

	// Public lookup — used by SlotManagement to validate patient and read specialty.
	// Returns pointer to patient by CNIC, or nullptr if not found.
	// Wraps the private findByCnic() to expose it safely.
	Patient* getPatientByCnic(long long cnic) {
		return findByCnic(cnic);
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
			getline(ss, token, '|'); p.setAge(stoi(token));
			getline(ss, token, '|'); p.setGender(token);
			getline(ss, token, '|'); p.setPhone(stoll(token));
			getline(ss, token, '|'); p.setDisease(token);
			getline(ss, token, '|'); p.setRequiredSpecialty(token);
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

		// Step 1 — confirm a bed is available before collecting any info
		double assignedCharge = 0.0;
		int bedIdx = bedManager.findFreeBed(assignedCharge);
		if(bedIdx == -1) {
			cout << "  [Error] No beds available. Cannot admit patient." << endl;
			return;
		}

		// Step 2 — collect all patient details via Person::collectBasicInfo()
		//           and disease menu via showDiseaseMenu() inside Patient::set()
		Patient p;
		p.set();

		// Step 3 — check for duplicate CNIC after input
		if(findIndexByCnic(p.getCnic()) != -1) {
			cout << "  [Error] A patient with this CNIC already exists." << endl;
			return;
		}

		// Step 4 — formally admit (assigns real ID) then assign bed
		p.markAdmitted(assignedCharge);
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
//  STRUCT: Slot
//  Represents one scheduled visit between a doctor and a patient.
//  Does not own either — holds their IDs only (aggregation).
//  Think of it as a calendar appointment card: it references
//  a doctor and a patient but neither belongs to it.
// ============================================================
struct Slot {
	int    slotId;      // unique slot identifier
	int    doctorId;    // which doctor  (aggregation — ID only)
	int    patientId;   // which patient (aggregation — ID only)
	time_t startTime;   // unix timestamp when visit begins
	time_t endTime;     // startTime + SLOT_DURATION_SECS
	bool   isCompleted; // true once the visit has taken place
};

const int SLOT_DURATION_SECS = 1800; // 30 minutes in seconds


// ============================================================
//  CLASS: SlotManagement
//  Owns all scheduled slots and enforces all scheduling rules.
//  Coordinates with DoctorManagement and PatientManagement
//  via association (references — does not own them).
//
//  Think of SlotManagement as a hospital's receptionist desk:
//  it knows every doctor's calendar and ensures no conflicts.
// ============================================================
class SlotManagement {
private:
	vector<Slot>       slots;          // owns all slot records (composition)
	DoctorManagement  &docManager;    // association — to fetch and validate doctors
	PatientManagement &patManager;    // association — to fetch and validate patients
	int                nextSlotId;    // auto-incrementing slot ID counter

	// Writes all slots to slots.txt, overwriting previous content.
	// First line stores the slot ID counter for persistence.
	void saveToFile() {
		ofstream file("slots.txt");
		if(!file.is_open()) {
			cout << "  [Error] Could not open slots.txt for writing." << endl;
			return;
		}
		file << nextSlotId << "\n";
		for(int i = 0; i < (int)slots.size(); i++) {
			const Slot &s = slots[i];
			file << s.slotId      << "|"
			     << s.doctorId    << "|"
			     << s.patientId   << "|"
			     << s.startTime   << "|"
			     << s.endTime     << "|"
			     << s.isCompleted << "\n";
		}
		file.close();
	}

	// Returns true if two time ranges [aStart,aEnd) and [bStart,bEnd) overlap.
	// Used to detect doctor double-booking.
	// Two ranges overlap when: aStart < bEnd AND bStart < aEnd
	// Think of it like two meetings on a calendar — they clash if either
	// starts before the other ends.
	bool timesOverlap(time_t aStart, time_t aEnd,
	                  time_t bStart, time_t bEnd) const {
		return (aStart < bEnd) && (bStart < aEnd);
	}

	// Returns true if a doctor already has a slot that overlaps the proposed window.
	// Enforces Rule 2 — a doctor cannot be in two places at the same time.
	bool doctorHasConflict(int doctorId,
	                       time_t propStart, time_t propEnd) const {
		for(int i = 0; i < (int)slots.size(); i++) {
			if(slots[i].doctorId == doctorId) {
				if(timesOverlap(propStart, propEnd,
				                slots[i].startTime, slots[i].endTime)) {
					return true;
				}
			}
		}
		return false;
	}

	// Returns true if a patient already has any slot today (pending or completed).
	// Enforces Rule 3 — one doctor visit per patient per day.
	// Think of it as a ward-round stamp: once the patient is visited today, done.
	bool patientVisitedToday(int patientId) const {
		time_t now       = time(nullptr);
		struct tm *today = localtime(&now);
		int todayDay     = today->tm_mday;
		int todayMon     = today->tm_mon;
		int todayYear    = today->tm_year;

		for(int i = 0; i < (int)slots.size(); i++) {
			if(slots[i].patientId == patientId) {
				struct tm *slotDay = localtime(&slots[i].startTime);
				if(slotDay->tm_mday  == todayDay  &&
				   slotDay->tm_mon   == todayMon  &&
				   slotDay->tm_year  == todayYear) {
					return true;
				}
			}
		}
		return false;
	}

public:
	// Constructor — takes references to shared managers (association)
	SlotManagement(DoctorManagement &dm, PatientManagement &pm)
		: docManager(dm), patManager(pm), nextSlotId(0) {
		cout << "  [System] SlotManagement initialised." << endl;
	}

	// Loads slots from slots.txt on startup.
	// First line restores the slot ID counter.
	void loadFromFile() {
		ifstream file("slots.txt");
		if(!file.is_open()) {
			cout << "  [Info] No slots file found. Starting fresh." << endl;
			return;
		}
		slots.clear();
		string line;

		// First line — restore slot ID counter
		if(getline(file, line) && !line.empty()) {
			nextSlotId = stoi(line);
		}

		// Remaining lines — restore each slot record
		while(getline(file, line)) {
			if(line.empty()) {
				continue;
			}
			stringstream ss(line);
			string token;
			Slot s;
			getline(ss, token, '|'); s.slotId      = stoi(token);
			getline(ss, token, '|'); s.doctorId    = stoi(token);
			getline(ss, token, '|'); s.patientId   = stoi(token);
			getline(ss, token, '|'); s.startTime   = (time_t)stoll(token);
			getline(ss, token, '|'); s.endTime     = (time_t)stoll(token);
			getline(ss, token, '|'); s.isCompleted = stoi(token);
			slots.push_back(s);
		}
		file.close();
		cout << "  [Info] Slots loaded: " << slots.size() << endl;
	}

	// Core scheduling method — auto-suggests free doctors by patient specialty.
	// Flow:
	//   1. Admin enters patient CNIC
	//   2. System reads requiredSpecialty from patient record
	//   3. Admin enters desired start hour (today, within shift)
	//   4. System applies 3 conflict rules per doctor
	//   5. Eligible doctors displayed — admin picks one
	//   6. Slot created and saved
	void assignSlot() {
		printHeader("ASSIGN SLOT");

		// Step 1 — identify patient
		cout << "  Enter patient CNIC: ";
		long long cnic = Validator::getCnic();
		Patient *p = patManager.getPatientByCnic(cnic);
		if(!p) {
			cout << "  [Error] Patient not found." << endl;
			return;
		}
		if(!p->getIsAdmitted() || p->getIsDischarged()) {
			cout << "  [Error] Patient is not currently admitted." << endl;
			return;
		}

		// Rule 3 — check if patient already visited today
		if(patientVisitedToday(p->id())) {
			cout << "  [Error] This patient has already been visited today." << endl;
			return;
		}

		string specialty = p->getRequiredSpecialty();
		cout << "  Patient: " << p->getName()
		     << "  |  Required specialty: " << specialty << endl;

		// Step 2 — get all doctors of matching specialty
		vector<Doctor*> candidates = docManager.getDoctorsBySpecialty(specialty);
		if(candidates.empty()) {
			cout << "  [Error] No doctors registered for specialty: "
			     << specialty << endl;
			return;
		}

		// Step 3 — ask admin for desired start hour
		cout << "  Enter desired start hour for today (9-18): ";
		int startHour = 0;
		while(true) {
			cin >> startHour;
			if(startHour >= 9 && startHour <= 18) {
				break;
			}
			cout << "  [Error] Hour must be between 9 and 18: ";
		}

		// Build today's timestamp for the requested hour
		time_t now       = time(nullptr);
		struct tm slotTm = *localtime(&now);
		slotTm.tm_hour   = startHour;
		slotTm.tm_min    = 0;
		slotTm.tm_sec    = 0;
		time_t propStart = mktime(&slotTm);
		time_t propEnd   = propStart + SLOT_DURATION_SECS;

		// Step 4 — apply Rule 1 (shift) and Rule 2 (conflict) per doctor
		vector<Doctor*> eligible;
		for(int i = 0; i < (int)candidates.size(); i++) {
			Doctor *d = candidates[i];

			// Rule 1 — is the requested hour within the doctor's shift?
			// A 30-min slot starting at startHour ends at startHour:30,
			// still within the same hour — so isOnDutyAt(startHour) is sufficient.
			// isOnDutyAt checks: hour >= shiftStart && hour < shiftEnd
			if(!d->isOnDutyAt(startHour)) {
				continue; // outside shift hours
			}

			// Rule 2 — does the doctor have a conflicting slot?
			if(doctorHasConflict(d->getDID(), propStart, propEnd)) {
				continue; // already booked at this time
			}

			eligible.push_back(d);
		}

		// Step 5 — show eligible doctors to admin
		if(eligible.empty()) {
			cout << "  [Error] No available doctors for specialty '"
			     << specialty << "' at " << startHour << ":00." << endl;
			cout << "  Try a different hour or check doctor schedules." << endl;
			return;
		}

		printLine();
		cout << "  Available doctors for " << specialty
		     << " at " << startHour << ":00:" << endl;
		printLine();
		for(int i = 0; i < (int)eligible.size(); i++) {
			cout << "  " << (i + 1) << ". Dr. " << eligible[i]->getName()
			     << "  (ID: " << eligible[i]->getDID() << ")"
			     << "  Shift: " << eligible[i]->getShiftStart()
			     << ":00 - "   << eligible[i]->getShiftEnd() << ":00"
			     << endl;
		}
		printLine();
		cout << "  Select doctor (1-" << eligible.size() << "): ";
		int pick = 0;
		while(true) {
			cin >> pick;
			if(pick >= 1 && pick <= (int)eligible.size()) {
				break;
			}
			cout << "  [Error] Enter a number between 1 and "
			     << eligible.size() << ": ";
		}

		// Step 6 — create and save the slot
		Doctor *chosen = eligible[pick - 1];
		Slot s;
		s.slotId      = ++nextSlotId;
		s.doctorId    = chosen->getDID();
		s.patientId   = p->id();
		s.startTime   = propStart;
		s.endTime     = propEnd;
		s.isCompleted = false;
		slots.push_back(s);
		saveToFile();

		// Confirmation display
		struct tm *st = localtime(&propStart);
		cout << "  [OK] Slot #" << s.slotId << " created." << endl;
		cout << "  Doctor  : Dr. " << chosen->getName() << endl;
		cout << "  Patient : "     << p->getName()      << endl;
		cout << "  Time    : "     << startHour << ":00 - "
		                           << startHour << ":30" << endl;
		(void)st; // suppress unused warning
	}

	// Marks a slot as completed by its ID.
	// Called after a doctor has finished their visit.
	void completeSlot() {
		printHeader("COMPLETE SLOT");
		cout << "  Enter Slot ID to mark as completed: ";
		int id = 0;
		cin >> id;

		for(int i = 0; i < (int)slots.size(); i++) {
			if(slots[i].slotId == id) {
				if(slots[i].isCompleted) {
					cout << "  [Error] Slot already marked as completed." << endl;
					return;
				}
				slots[i].isCompleted = true;
				saveToFile();
				cout << "  [OK] Slot #" << id << " marked as completed." << endl;
				return;
			}
		}
		cout << "  [Error] Slot ID not found." << endl;
	}

	// Displays all slots (pending and completed) for a specific doctor.
	// Admin enters doctor ID — system lists every appointment on record.
	void viewDoctorSchedule() {
		printHeader("DOCTOR SCHEDULE");
		cout << "  Enter Doctor ID: ";
		int docId = 0;
		cin >> docId;

		Doctor *d = docManager.getDoctorById(docId);
		if(!d) {
			cout << "  [Error] Doctor ID not found." << endl;
			return;
		}

		cout << "  Schedule for Dr. " << d->getName()
		     << " (" << d->getSpecialty() << ")" << endl;
		printLine();

		bool found = false;
		for(int i = 0; i < (int)slots.size(); i++) {
			if(slots[i].slotId && slots[i].doctorId == docId) {
				found = true;
				struct tm *st = localtime(&slots[i].startTime);
				struct tm *en = localtime(&slots[i].endTime);
				cout << "  Slot #"    << slots[i].slotId
				     << "  Patient #" << slots[i].patientId
				     << "  Start: "   << st->tm_hour << ":" << st->tm_min
				     << "  End: "     << en->tm_hour << ":" << en->tm_min
				     << "  Status: "  << (slots[i].isCompleted ? "Completed"
				                                                : "Pending")
				     << endl;
			}
		}
		if(!found) {
			cout << "  No slots found for this doctor." << endl;
		}
		printLine();
	}

	// Displays all visits (pending and completed) received by a specific patient.
	// Admin enters patient CNIC — system lists every slot on record.
	void viewPatientVisits() {
		printHeader("PATIENT VISITS");
		cout << "  Enter patient CNIC: ";
		long long cnic = Validator::getCnic();

		Patient *p = patManager.getPatientByCnic(cnic);
		if(!p) {
			cout << "  [Error] Patient not found." << endl;
			return;
		}

		cout << "  Visit history for: " << p->getName() << endl;
		printLine();

		bool found = false;
		for(int i = 0; i < (int)slots.size(); i++) {
			if(slots[i].patientId == p->id()) {
				found = true;
				Doctor *d     = docManager.getDoctorById(slots[i].doctorId);
				struct tm *st = localtime(&slots[i].startTime);
				cout << "  Slot #"   << slots[i].slotId
				     << "  Doctor: " << (d ? d->getName() : "Unknown")
				     << "  Time: "   << st->tm_hour << ":" << st->tm_min
				     << "  Status: " << (slots[i].isCompleted ? "Completed"
				                                              : "Pending")
				     << endl;
			}
		}
		if(!found) {
			cout << "  No visits recorded for this patient." << endl;
		}
		printLine();
	}

	// Removes a slot permanently by its ID.
	// Only pending slots can be removed — completed visits are kept for records.
	// Think of it as cancelling an appointment before it happens.
	void removeSlot() {
		printHeader("REMOVE SLOT");
		cout << "  Enter Slot ID to remove: ";
		int id = 0;
		cin >> id;

		for(int i = 0; i < (int)slots.size(); i++) {
			if(slots[i].slotId == id) {
				if(slots[i].isCompleted) {
					cout << "  [Error] Cannot remove a completed slot — "
					     << "visit already recorded." << endl;
					return;
				}
				slots.erase(slots.begin() + i);
				saveToFile();
				cout << "  [OK] Slot #" << id << " removed." << endl;
				return;
			}
		}
		cout << "  [Error] Slot ID not found." << endl;
	}

	// Main menu loop for slot management
	void slotMenu() {
		int choice = 0;
		while(choice != 6) {
			printHeader("SLOT MANAGEMENT");
			cout << "  1. Assign Slot"           << endl;
			cout << "  2. Complete a Slot"       << endl;
			cout << "  3. Remove a Slot"         << endl;
			cout << "  4. View Doctor Schedule"  << endl;
			cout << "  5. View Patient Visits"   << endl;
			cout << "  6. Back"                  << endl;
			printLine();
			cout << "  Choice: ";
			cin >> choice;
			switch(choice) {
				case 1: assignSlot();          break;
				case 2: completeSlot();        break;
				case 3: removeSlot();          break;
				case 4: viewDoctorSchedule();  break;
				case 5: viewPatientVisits();   break;
				case 6:                        break;
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
	DoctorManagement  doctorManager;
	PatientManagement patientManager;
	SlotManagement    slotManager;

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
		: patientManager(bedManager, medManager),
		  slotManager(doctorManager, patientManager)
	{
		user.uID = defaultUserId;
		user.pwd = defaultPassword;
		cout << "  [System] HMS initialised." << endl;

		// Load order matters:
		// beds + medicines before patients, doctors before slots
		bedManager.loadFromFile();
		medManager.loadFromFile();
		doctorManager.loadFromFile();
		patientManager.loadPatientsFromFile();
		patientManager.loadMedicationFromFile();
		slotManager.loadFromFile();
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
		while(choice != 6) {
			printHeader("HOSPITAL MANAGEMENT SYSTEM");
			cout << "  1. Patient Management"  << endl;
			cout << "  2. Medicine Management" << endl;
			cout << "  3. Bed Management"      << endl;
			cout << "  4. Doctor Management"   << endl;
			cout << "  5. Slot Management"     << endl;
			cout << "  6. Exit"                << endl;
			printLine();
			cout << "  Choice: ";
			cin >> choice;

			switch(choice) {
				case 1: patientManager.patientManagementMenu(); break;
				case 2: medManager.medicineMenu();              break;
				case 3: bedManager.bedMenu();                   break;
				case 4: doctorManager.doctorMenu();             break;
				case 5: slotManager.slotMenu();                 break;
				case 6:
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