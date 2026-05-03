# 🏥 Hospital Management System (HMS)

> A console-based C++ application for managing hospital operations — patients, doctors, beds, medicines, and scheduled appointments — with full disk persistence across every session.

**Author:** Dawood Imran &nbsp;|&nbsp; **Language:** C++14 &nbsp;|&nbsp; **Platform:** Windows (uses `conio.h`) &nbsp;|&nbsp; **Lines of Code:** ~2,200

---

## Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [Project Structure](#project-structure)
4. [Architecture & Class Hierarchy](#architecture--class-hierarchy)
5. [OOP Relationships](#oop-relationships)
6. [Modules In Detail](#modules-in-detail)
   - [Validator](#-validator)
   - [Person (Base Class)](#-person-base-class)
   - [Patient](#-patient--person)
   - [Doctor](#-doctor--person)
   - [BedManagement](#-bedmanagement)
   - [MedManagement](#-medmanagement)
   - [DoctorManagement](#-doctormanagement)
   - [PatientManagement](#-patientmanagement)
   - [SlotManagement](#-slotmanagement)
   - [HMS (Root Controller)](#-hms-root-controller)
7. [Data Persistence](#data-persistence)
8. [Disease & Specialty System](#disease--specialty-system)
9. [Scheduling Logic](#scheduling-logic)
10. [Login System](#login-system)
11. [Getting Started](#getting-started)
12. [File Format Reference](#file-format-reference)
13. [Known Limitations](#known-limitations)

---

## Overview

The Hospital Management System is a fully object-oriented C++ application built around six core modules that mirror real hospital operations. Every piece of data survives program restarts — all records are written to flat pipe-delimited text files after every change.

The system enforces real-world constraints: beds can't be double-assigned, medicines can't go below zero stock, a doctor can't be in two places at once, and a patient can only be visited once per day. These rules are not just UI guards — they are enforced at the class level.

---

## Features

| Module | Capabilities |
|---|---|
| **Patients** | Admit, discharge, remove, manage medication, generate bill |
| **Doctors** | Register, view roster, remove, specialty assignment |
| **Beds** | Auto-assign on admission, release on discharge, add/remove beds |
| **Medicines** | Add inventory, update stock, prescribe to patients |
| **Slots** | Schedule doctor visits, conflict detection, completion tracking |
| **Login** | Masked password input, 3-attempt lockout |

---

## Project Structure

```
HMS/
│
├── HMS.cpp               ← entire application in one file
│
└── Runtime data files (auto-created on first run)
    ├── beds.txt          ← bed inventory and reservation state
    ├── medicines.txt     ← pharmacy inventory
    ├── patients.txt      ← patient records + admission state
    ├── medications.txt   ← per-patient medication history
    ├── doctors.txt       ← doctor roster and shift info
    └── slots.txt         ← scheduled and completed visit slots
```

> **Note:** All `.txt` files are created automatically. Delete them to reset the system to a fresh state. If upgrading from a previous version of this code, delete `patients.txt` as the file format has changed.

---

## Architecture & Class Hierarchy

```
                        ┌─────────────┐
                        │    Person   │  ← Abstract Base Class
                        │  (abstract) │
                        └──────┬──────┘
                               │ inherits
               ┌───────────────┴────────────────┐
               │                                │
        ┌──────┴──────┐                 ┌───────┴──────┐
        │   Patient   │                 │    Doctor    │
        │ + disease   │                 │ + specialty  │
        │ + pID       │                 │ + shift hours│
        │ + meds[]    │                 │ + dID        │
        └─────────────┘                 └──────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                         HMS (Root)                              │
│                                                                 │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────────┐  │
│  │BedManagement │  │MedManagement │  │  DoctorManagement    │  │
│  │ owns Bed[]   │  │owns Medicine[]│  │   owns Doctor[]      │  │
│  └──────────────┘  └──────────────┘  └──────────────────────┘  │
│                                                                 │
│  ┌───────────────────────────────┐  ┌──────────────────────┐   │
│  │      PatientManagement        │  │   SlotManagement     │   │
│  │       owns Patient[]          │  │    owns Slot[]       │   │
│  │  refs → BedManagement         │  │ refs → DoctorMgmt    │   │
│  │  refs → MedManagement         │  │ refs → PatientMgmt   │   │
│  └───────────────────────────────┘  └──────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

---

## OOP Relationships

Understanding the three types of relationships used throughout this codebase:

### Inheritance
`Patient` and `Doctor` both inherit from `Person`. Think of `Person` as a government ID card template — every person has a name, CNIC, age, gender, and phone. What makes a doctor a doctor (specialty, shift) and a patient a patient (disease, admission) is added on top.

```
Person  →  shared identity (name, cnic, age, gender, phone)
Patient →  adds medical context (disease, admission, billing)
Doctor  →  adds professional context (specialty, shift hours)
```

### Composition — *"owns completely"*
A management class fully owns its objects. When `PatientManagement` is destroyed, all `Patient` objects go with it. A `Patient` fully owns its `vector<Medication>` — medications have no meaning outside that patient.

```
PatientManagement  owns  vector<Patient>
DoctorManagement   owns  vector<Doctor>
BedManagement      owns  vector<Bed>
SlotManagement     owns  vector<Slot>
Patient            owns  vector<Medication>
```

### Aggregation — *"references, doesn't own"*
A `Slot` holds a `doctorId` and a `patientId` but does not own either. If the slot is deleted, the doctor and patient still exist. They are linked by ID, like a booking receipt that references a room number.

```
Slot.doctorId   →  references Doctor  (by ID)
Slot.patientId  →  references Patient (by ID)
```

### Association — *"uses temporarily"*
`SlotManagement` holds references to `DoctorManagement` and `PatientManagement`. It uses them to validate and look up records but does not own them. If `SlotManagement` were destroyed, the managers would be unaffected.

```
SlotManagement    uses  DoctorManagement  (ref)
SlotManagement    uses  PatientManagement (ref)
PatientManagement uses  BedManagement     (ref)
PatientManagement uses  MedManagement     (ref)
```

---

## Modules In Detail

### 🔍 Validator

**Location:** Top of file, before all classes.

A utility class with only `static` methods — it is never instantiated in real use. Provides validated, looping input for every data type used in the system.

| Method | Returns | Rule |
|---|---|---|
| `getCnic()` | `long long` | Exactly 13 digits, numeric only |
| `getPhone()` | `long long` | Exactly 11 digits, numeric only |
| `getTextOnly()` | `string` | Letters and spaces only, non-empty |
| `getPositiveInt()` | `int` | Greater than zero |
| `getPositiveDouble()` | `double` | Greater than zero |

Every method loops indefinitely until valid input is received — the user can never proceed with garbage data.

---

### 👤 Person (Base Class)

**Type:** Abstract — cannot be instantiated directly.

The shared foundation for `Doctor` and `Patient`. Declaring `set()` and `display()` as pure virtual (`= 0`) forces every derived class to implement them or remain abstract itself.

```cpp
protected:
    string    name;
    long long cnic;
    int       age;
    string    gender;
    long long phone;

    void collectBasicInfo();   // shared input helper used by derived set()

public:
    virtual void set()     = 0;  // pure virtual
    virtual void display() = 0;  // pure virtual
    virtual ~Person()      {}    // virtual destructor — always required
```

**Why `protected` fields?** `private` would lock derived classes out entirely. `protected` gives `Doctor` and `Patient` direct access to these fields while keeping them hidden from outside code — the family safe analogy.

**Why virtual destructor?** Without it, deleting a `Patient*` through a `Person*` pointer would only call `Person`'s destructor, leaking the derived class's resources.

---

### 🛏 Patient `: public Person`

**Adds:** Medical context — disease, admission state, billing, medications.

Key design decisions:

- `static int nextId` — a class-level counter shared across all instances. Incremented only inside `markAdmitted()`, after a bed is confirmed, so IDs are never wasted on failed admissions.
- `showDiseaseMenu()` — private helper that replaces free-text disease input with a numbered menu. Sets both `disease` and `requiredSpecialty` in one step.
- `restoreState()` — called during file loading to bypass the normal admission flow and directly restore all private fields.

```
Patient lifecycle:
  Constructor → set() → markAdmitted() → [addMedication()...] → markDischarged() → generateBill()
```

**Bill calculation:** `days stayed × bedCharge + Σ(medicineQty × unitPrice)`. Minimum one day charged even for same-day stays.

---

### 👨‍⚕️ Doctor `: public Person`

**Adds:** Professional context — specialty, shift hours, duty check.

Key design decisions:

- `showSpecialtyMenu()` — mirrors the disease menu. Picks from the same 5 specialties used in the disease list, guaranteeing specialty strings always match.
- `inputShiftHours()` — validates start (9–17) and end (10–19) with the rule that end must exceed start. Stored as plain integers (hours in 24h format).
- `isOnDutyAt(int hour)` — the clean duty-check used by `SlotManagement`. Returns `hour >= shiftStart && hour < shiftEnd`.
- ID is assigned by `DoctorManagement::addDoctor()` after the duplicate CNIC check — not inside `set()`. This prevents wasting the ID counter on rejected registrations.

---

### 🛏 BedManagement

Owns and manages the hospital's bed inventory. On first run with no `beds.txt`, creates 10 default beds at Rs. 1000/day.

**Key behaviors:**
- `findFreeBed()` — scans for first unreserved bed, returns its index and charge. Does NOT reserve it — the caller commits after collecting patient info.
- `assignBedByIndex()` — reserves a specific bed by vector index.
- `resignBed()` — scans by patient ID and releases whichever bed they hold.
- `removeBed()` — refuses to remove reserved beds. Re-indexes remaining beds after removal.

---

### 💊 MedManagement

Owns the pharmacy inventory. Starts empty if no `medicines.txt` exists.

**Key behavior — `consumeMedicine()`:** The only method that modifies stock from outside. Takes a medicine ID, quantity, and an output `Medication` struct. Deducts stock and fills the struct with a snapshot of the medicine's details at time of prescription (price locks in at prescription time, not at billing time).

---

### 🩺 DoctorManagement

Owns the doctor roster. Persists to `doctors.txt`.

**Specialty-based lookup for SlotManagement:**
- `getDoctorById(int id)` — returns `Doctor*`, used during slot display to show doctor name.
- `getDoctorsBySpecialty(string)` — returns `vector<Doctor*>`, the entry point for auto-suggest scheduling.

---

### 🧑‍💼 PatientManagement

The most coordinated class — references `BedManagement` and `MedManagement` to perform admission and discharge.

**Admission flow (`addPatient()`):**
1. Check bed availability first — abort early if none free
2. Call `patient.set()` to collect all info including disease menu
3. Check for duplicate CNIC after input
4. Call `markAdmitted()` to lock in the timestamp and assign ID
5. Assign the confirmed bed to the patient's real ID
6. Push to vector and save

**Two separate save files:**
- `patients.txt` — identity and admission state
- `medications.txt` — medication history (foreign key: patientId)

This separation keeps the patient file clean and allows medications to be updated independently without rewriting the entire patient record structure.

---

### 📅 SlotManagement

The scheduling brain of the system. Associates with both `DoctorManagement` and `PatientManagement` via references.

**Three scheduling rules enforced on every `assignSlot()` call:**

| Rule | Scope | Description |
|---|---|---|
| **Rule 1 — Shift boundary** | Per doctor | `isOnDutyAt(requestedHour)` must return true |
| **Rule 2 — No double booking** | Per doctor | No time overlap with existing slots |
| **Rule 3 — One visit per day** | Per patient | Patient can only be visited once per calendar day |

Rules 1 and 2 filter individual doctors — one doctor may be busy while another of the same specialty is free. Rule 3 is a global patient-level block.

**Overlap detection formula:**
```
Two ranges [A,B] and [C,D] overlap if:  A < D  AND  C < B
```

**`removeSlot()`** only allows removal of pending slots. Completed visit records are protected and cannot be deleted — preserving the audit trail.

---

### 🏥 HMS (Root Controller)

Owns all subsystems and is the only entry point. Handles login before exposing the main menu.

**Initialisation order matters:**
```cpp
bedManager.loadFromFile();          // 1. beds first
medManager.loadFromFile();          // 2. medicines
doctorManager.loadFromFile();       // 3. doctors
patientManager.loadPatientsFromFile();   // 4. patients (references beds+meds)
patientManager.loadMedicationFromFile(); // 5. medications (references patients)
slotManager.loadFromFile();         // 6. slots last (references doctors+patients)
```

Loading out of order would cause lookups to fail on orphaned IDs.

---

## Data Persistence

Every change is written to disk immediately after it occurs — there is no "save" step and no risk of data loss on exit.

All files use pipe (`|`) as a delimiter. The first line of files with ID counters stores the counter value so IDs remain globally unique across restarts.

```
patients.txt format:
nextId
pID|name|cnic|age|gender|phone|disease|specialty|admitted|discharged|admitTime|dischargeTime|bedCharge

doctors.txt format:
nextId
dID|name|cnic|age|gender|phone|specialty|shiftStart|shiftEnd

slots.txt format:
nextSlotId
slotId|doctorId|patientId|startTime|endTime|isCompleted

medications.txt format:
patientId|medicineId|name|type|quantity|price

beds.txt format:
bId|patientId|reserved|bedCharge

medicines.txt format:
mID|name|type|stock|pricePerItem
```

---

## Disease & Specialty System

Replaces free-text disease input with a fixed numbered menu. Guarantees specialty strings on patients always match specialty strings on doctors — no typos, no mismatches.

| # | Disease | Specialty |
|---|---|---|
| 1 | Flu | General |
| 2 | Fever | General |
| 3 | Typhoid | General |
| 4 | Hypertension | Cardiology |
| 5 | Heart Disease | Cardiology |
| 6 | Epilepsy | Neurology |
| 7 | Migraine | Neurology |
| 8 | Fracture | Orthopedics |
| 9 | Joint Pain | Orthopedics |
| 10 | Asthma | Pulmonology |

When a patient is admitted with disease #4 (Hypertension), `patient.requiredSpecialty` is automatically set to `"Cardiology"`. `SlotManagement` reads this field to find matching doctors.

---

## Scheduling Logic

The full `assignSlot()` flow:

```
1. Admin enters patient CNIC
        ↓
2. System reads patient.requiredSpecialty
        ↓
3. DoctorManagement returns all doctors with matching specialty
        ↓
4. Admin enters desired start hour (9–18)
        ↓
5. For each candidate doctor:
   ├── Rule 1: isOnDutyAt(hour)?        No → skip
   └── Rule 2: doctorHasConflict()?     Yes → skip
        ↓
6. Rule 3: patientVisitedToday()?       Yes → reject entirely
        ↓
7. Show eligible doctors → admin picks one
        ↓
8. Slot created: startTime = today@hour:00
                 endTime   = startTime + 1800 seconds (30 min)
        ↓
9. Saved to slots.txt immediately
```

---

## Login System

```
Default credentials:
  ID      : 4040
  Password: Admin
```

- Password input uses `_getch()` from `<conio.h>` — characters are masked as `*`
- Backspace correction supported during typing
- 3 attempts allowed before session is locked
- Credentials are set in the `HMS` constructor and can be changed there

---

## Getting Started

### Requirements
- Windows OS (required for `<conio.h>` and `_getch()`)
- C++14 compatible compiler (MSVC, MinGW, or Clang on Windows)

### Compile & Run

```bash
# Using g++ (MinGW)
g++ -std=c++14 -o HMS HMS.cpp

# Run
./HMS.exe
```

### First Run
On first run with no data files present:
- 10 default beds are created at Rs. 1000/day
- Medicine inventory starts empty — add medicines before admitting patients
- No doctors are registered — add at least one doctor per specialty before assigning slots

### Recommended Setup Order
```
1. Bed Management    → verify default beds or add more
2. Medicine Mgmt     → add medicines to inventory
3. Doctor Mgmt       → register doctors with specialties and shifts
4. Patient Mgmt      → admit patients (disease menu auto-assigns specialty)
5. Slot Management   → assign doctor visits to admitted patients
```

---

## File Format Reference

Quick reference for manually inspecting or recovering data files:

```
beds.txt
  Line 1+: bId | patientId(0=empty) | reserved(0/1) | chargePerDay

medicines.txt
  Line 1+: mID | name | type | stock | pricePerUnit

patients.txt
  Line 1:  nextId (ID counter)
  Line 2+: pID | name | cnic | age | gender | phone | disease | specialty
           | admitted | discharged | admitTime | dischargeTime | bedCharge

medications.txt
  Line 1+: patientId | medicineId | name | type | quantity | unitPrice

doctors.txt
  Line 1:  nextId (ID counter)
  Line 2+: dID | name | cnic | age | gender | phone | specialty
           | shiftStart | shiftEnd

slots.txt
  Line 1:  nextSlotId (ID counter)
  Line 2+: slotId | doctorId | patientId | startTime | endTime | isCompleted
```

All timestamps are Unix epoch integers (`time_t`). `admitted`, `reserved`, `isCompleted` are stored as `0` or `1`.

---

## Known Limitations

- **Windows only** — `<conio.h>` and `_getch()` are not available on Linux/macOS. Replace with a platform-agnostic password masking approach to port.
- **Single admin account** — credentials are hardcoded in the `HMS` constructor. No multi-user support.
- **No slot editing** — a slot can be removed (if pending) or completed, but its time or doctor cannot be changed after creation.
- **File format versioning** — upgrading from a previous build requires deleting data files, as the format changed when `Person` fields were added to the patient record.
- **No search** — patients and doctors can only be looked up by CNIC or ID. There is no name-based search.

---

*Built as a learning project demonstrating OOP principles: inheritance, composition, aggregation, association, virtual dispatch, static members, and file I/O in C++.*
