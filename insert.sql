-- Drop existing tables
DROP TABLE Incident;
DROP TABLE Gear;
DROP TABLE Transactions;
DROP TABLE Offer;
DROP TABLE Brands;

-- Create Brands Table 
CREATE TABLE Brands (
comp_id CHAR(2) PRIMARY KEY,
b_name VARCHAR(255)
);

-- Insert Data Into Brands
-- Brand Id's all start with zero for test purposes
INSERT INTO Brands (comp_id, b_name) VALUES ('01', 'Bauer');
INSERT INTO Brands (comp_id, b_name) VALUES ('02', 'CCM');
INSERT INTO Brands (comp_id, b_name) VALUES ('03', 'True');
INSERT INTO Brands (comp_id, b_name) VALUES('04', 'Sherwood');

----------------------------------------------

-- Create Table Offers
CREATE TABLE Offer (
offer_id CHAR(2) PRIMARY KEY,
comp_id CHAR(2),
gear_type VARCHAR(255),
price DECIMAL(10, 2),
FOREIGN KEY (comp_id) REFERENCES Brands(comp_id)
);

-- Insert Data Into Offers
-- Offer Id's all start with one for test purposes
INSERT INTO Offer (offer_id, comp_id, gear_type, price) VALUES ('11', '01', 'Hockey Stick', 129.99);
INSERT INTO Offer (offer_id, comp_id, gear_type, price) VALUES ('12', '02', 'Goalie Mask', 299.99);
INSERT INTO Offer (offer_id, comp_id, gear_type, price) VALUES ('13', '03', 'Hockey Skates', 199.99);
INSERT INTO Offer (offer_id, comp_id, gear_type, price) VALUES ('14', '04', 'Hockey Gloves', 89.99);

----------------------------------------------

-- Create Transactions Table
CREATE TABLE Transactions (
trans_id CHAR(2) PRIMARY KEY,
offer_id CHAR(2),
quantity INT,
price DECIMAL(10, 2),
purchase_date DATE,
FOREIGN KEY (offer_id) REFERENCES Offer(offer_id)
);

-- Insert Into Transactions
-- Transaction Id's all start with two for test purposes
INSERT INTO Transactions (trans_id, offer_id, quantity, price, purchase_date) VALUES ('22', '12', 2, 299.99, TO_DATE('2024-04-12', 'YYYY-MM-DD'));
INSERT INTO Transactions (trans_id, offer_id, quantity, price, purchase_date) VALUES ('23', '13', 3, 199.99, TO_DATE('2024-04-11', 'YYYY-MM-DD'));
INSERT INTO Transactions (trans_id, offer_id, quantity, price, purchase_date) VALUES ('24', '14', 4, 89.99, TO_DATE('2024-04-10', 'YYYY-MM-DD'));

----------------------------------------------

-- Create Gear Table
CREATE TABLE Gear (
gear_id CHAR(2) PRIMARY KEY,
gear_type VARCHAR(255),
gear_size VARCHAR(4),
colour VARCHAR(50),
num_available INT
);

-- Insert into gear table
-- Gear Id's all start with three for test purposes
INSERT INTO Gear (gear_id, gear_type, gear_size, colour, num_available) VALUES ('31', 'Hockey Stick', 'XL', 'Black', 20);
INSERT INTO Gear (gear_id, gear_type, gear_size, colour, num_available) VALUES ('32', 'Goalie Mask', 'L', 'White', 15);
INSERT INTO Gear (gear_id, gear_type, gear_size, colour, num_available) VALUES ('33', 'Hockey Skates', '10', 'Black', 30);
INSERT INTO Gear (gear_id, gear_type, gear_size, colour, num_available) VALUES ('34', 'Hockey Gloves', 'XXL', 'Black', 25);

----------------------------------------------

-- Create Incidents Table
CREATE TABLE Incident (
inc_id CHAR(2) PRIMARY KEY,
gear_id CHAR(2),
i_description VARCHAR(255),
date_reported DATE,
FOREIGN KEY (gear_id) REFERENCES Gear(gear_id)
);

-- Fill Incidents Table
-- Gear Id's all start with four for test purposes
INSERT INTO Incident (inc_id, gear_id, i_description, date_reported) VALUES ('41', '31', 'Cracked blade', TO_DATE('2024-04-13', 'YYYY-MM-DD'));
INSERT INTO Incident (inc_id, gear_id, i_description, date_reported) VALUES ('42', '32', 'Strap broken', TO_DATE('2024-04-12', 'YYYY-MM-DD'));
INSERT INTO Incident (inc_id, gear_id, i_description, date_reported) VALUES ('43', '33', 'Blade chipped', TO_DATE('2024-04-11', 'YYYY-MM-DD'));
INSERT INTO Incident (inc_id, gear_id, i_description, date_reported) VALUES ('44', '34', 'Torn palm', TO_DATE('2024-04-10', 'YYYY-MM-DD'));
