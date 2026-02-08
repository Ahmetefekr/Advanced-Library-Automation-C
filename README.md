# 📚 Advanced Library Automation System in C

> A robust, file-based library management system that simulates a relational database using **Doubly Linked Lists**, **Dynamic Arrays**, and **CSV persistence** without any SQL engine.

![C](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Data Structures](https://img.shields.io/badge/Data_Structures-Doubly_Linked_List-red?style=for-the-badge)
![Database](https://img.shields.io/badge/Storage-Custom_CSV_Db-yellow?style=for-the-badge)

## 📖 Project Overview
This project was developed to demonstrate low-level memory management and algorithm design capabilities in C. Unlike simple automation tools, this system manages complex **Many-to-Many relationships** between Books and Authors and handles dynamic memory allocation manually.

It features a custom-built "database engine" that reads/writes to CSV files, ensuring data persistence while strictly using **Pointer Arithmetic** and **Structs**.

## ⚙️ Key Technical Features

### 1. Advanced Data Structures
The system moves beyond static arrays, utilizing dynamic structures for memory efficiency:
* **Doubly Linked Lists (`prev`, `next`):** Implemented for the `Student` (Ogrenci) module to allow efficient bidirectional traversal.
* **Singly Linked Lists:** Used for `Author` and `Book` dynamic storage.
* **Dynamic Arrays (`realloc`):** Used for the `KitapYazar` mapping table to handle scalable relationship data.
* ]**Struct-based Modeling:** Complex real-world entities modeled purely with C structs.

### 2. Custom Relational Database Logic
The system mimics a relational database using strict file operations:
* **Relational Mapping:** Links Books to Authors via a dedicated mapping structure (`KitapYazar`), simulating a JOIN operation.
* **Persistence:** All data is instantly serialized to CSV files (`Ogrenciler.csv`, `kitaplar.csv`, etc.), ensuring data integrity on program exit.

### 3. Core Capabilities
* **Borrowing Logic:** Tracks borrow/return dates and automatically calculates duration.
* **Penalty System:** Automatically deducts **10 points** from students who return books after 15 days.
* **Smart Inventory:** Tracks multiple physical copies of the same book (e.g., `ISBN_1`, `ISBN_2`) and their individual status ("RAFTA" or "Borrowed").

## 📂 Data Structure (CSV Format)
The system operates with specific CSV file formats. Ensure these files exist in the root directory for the program to load data correctly:

| File Name | Description | Format |
| :--- | :--- | :--- |
| **Ogrenciler.csv** | Student Data | `No,Name,Surname,Score` |
| **yazarlar.csv** | Author Data | `ID,Name,Surname` |
| **kitaplar.csv** | Book Inventory | `Name,ISBN,LabelID,Status` |
| **KitapYazar.csv** | Relations | `ISBN,AuthorID` |
| **kitapodunc.csv** | Transaction Log | `LabelID,StudentNo,Type,Date` |

## 📸 Functionality
The application runs on a CLI (Command Line Interface) menu system allowing:
* Student Management: Add/Delete/Update via Linked List operations.
* Book & Author Management: Full CRUD operations.
* Borrow/Return Operations: Real-time date input and validation.
* Reporting: List students with penalties or unreturned books.

**Developed by Ahmet Efe Karahan as a Computer Engineering Capstone Project (BLM1031).**
