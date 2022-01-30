#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctype.h>
#include <sstream>  
#include <map>
#include <bitset>
#include <iomanip>
#include <cstdlib>

using namespace std;

//Global Variables
int registers[32];
unsigned int PC = 0;
map<unsigned int, string> instructs;
map<string, unsigned int> labels;
map<unsigned int, int> memory;

const string WHITESPACE = " \n\r\t\f\v";

bool get_label_instrct(string file_name); //reads instructions

//---Instructions functions
void lui(int rd, int imm);
void jal(int rd, string label);
void jalr(int rd, int offset, int base);
void auipc(int rd, int imm);
void add(int x, int y, int z);
void sub(int x, int y, int z);
void sll(int x, int y, int z);
void srl(int x, int y, int z); //unsigned only
void sra(int x, int y, int z);
void slt(int x, int y, int z);
void sltu(int x, int y, int z);
void and_(int x, int y, int z);
void or_(int x, int y, int z);
void xor_(int x, int y, int z);
/////////////////////////////////////////////////////////Itype
void addi(int x, int y, int z);
void slti(int x, int y, int z);
void sltiu(int x, int y, int z);
void xori(int x, int y, int z);
void andi(int x, int y, int z);
void ori(int x, int y, int z);
void slli(int x, int y, int z);
void srli(int x, int y, int z);
void srai(int x, int y, int z);

//-----Reading instructions
void readInstruction(string tempS);


//types
vector<string> Rtype(string s);
vector<string> Itype(string s);
vector<string> UJtype(string s);
vector<string> Stype(string s);
vector<string> Btype(string s);



//Utility functions
string trim(const string& s);
string ltrim(const string& s);
string rtrim(const string& s);
bool validpc(string s);
string lower(string s); //makes the instruciton in lower case.
int map_reg(string s);
void output(char format);

int main() {

	cout << "Welcome!\n";
	cout << "Please enter the starting PC address (a non negative decimal integer not greater than 1e9)\n";

	//intilize PC
	cout << "PC: ";
	string pc_s;
	cin >> pc_s;
	while (!validpc(pc_s)) {
		cout << "Please enter a valid PC.\n";
		cout << "PC: ";
		cin >> pc_s;
	}
	PC = stoll(pc_s);


	//initilize registers to zero
	for (int i = 0; i < 32; i++) {
		registers[i] = 0;
	}

	string file_name;
	cout << "Please enter the name of the instructions file (with extension, Ex: instructions.txt)\n";
	cout << "File name: ";
	cin >> file_name;
	//initilize the map of instructions and read instructions file
	while (!get_label_instrct(file_name)) {
		cout << "File name: ";
		cin >> file_name;
	}

	//initilize the memory map and read the memory file
	cout << "Please enter the name of the memory file (with extension, ex: memory.txt)\n";
	cout << "File name: ";
	cin >> file_name;
	ifstream memoryFile(file_name);
	while (!memoryFile.is_open()) {
		cout << "File couldn't be found please check again.\n";
		cout << "File name: ";
		cin >> file_name;
		memoryFile.open(file_name);
	}
	int address;
	int value;
	while (memoryFile >> address) {
		memoryFile >> value;
		memory.insert(pair<int, int>(address, value));
	}

	cout << "Please choose the format of the output.\n";
	cout << "For hexadecimal type 'h', for decimal type 'd', for binary type 'b' all without quotations.\n";
	cout << "Format: ";
	char format;
	cin >> format;
	while (format != 'h' && format != 'd' && format != 'b') {
		cout << "Wrong fromat, please try again.\n";
		cout << "Format: ";
		cin >> format;
	}

	while (lower(instructs[PC].substr(0, 5)) != "fence" && lower(instructs[PC]) != "ecall" && lower(instructs[PC]) != "ebreak") {
		cout << dec << "PC: " << PC << endl;
		cout << "Instruction: " << instructs[PC] << endl;
		readInstruction(instructs[PC]);
		output(format);
	}
	cout << "*******End of Program*******\n";
	memoryFile.close();
	system("pause");
	return 0;
}



void output(char f) {
	cout << "printing registers: " << endl;
	if (f == 'h') {
		for (int i = 0; i < 8; i++) {
			cout << setfill(' ') << left << "x" << dec << setw(2) << 4 * i << ": "
				<< "0x" << setfill('0') << setw(8) << right << hex << registers[4 * i]
				<< setfill(' ') << left << "      x" << dec << setw(2) << 4 * i + 1 << ": "
				<< "0x" << setfill('0') << setw(8) << right << hex << registers[4 * i + 1]
				<< setfill(' ') << left << "      x" << dec << setw(2) << 4 * i + 2 << ": "
				<< "0x" << setfill('0') << setw(8) << right << hex << registers[4 * i + 2]
				<< setfill(' ') << left << "       x" << dec << setw(2) << 4 * i + 3 << ": "
				<< "0x" << setfill('0') << setw(8) << right << hex << registers[4 * i + 3] << endl;
		}
		cout << endl;
		cout << "printing memory: " << endl;
		map<unsigned int, int>::iterator it;
		int line = 0;
		for (it = memory.begin(); it != memory.end(); it++) {
			cout << "0x" << setfill('0') << setw(8) << right << hex << it->first
				<< ": " << "0x" << setfill('0') << setw(8) << right << hex << it->second << "       ";
			if (line == 2)
				cout << endl;
			line = (line + 1) % 3;
		}
		if (line != 2) cout << endl;
		cout << endl;

	} else if (f == 'd') {
		int line = 0;
		for (int i = 0; i < 32; i++) {
			cout << left << "x" << setw(2) << i << ": "
				<< setw(12) << registers[i];
			if (line == 3)
				cout << endl;
			line = (line + 1) % 4;
		}
		cout << endl;
		line = 0;
		cout << "printing memory: " << endl;
		map<unsigned int, int>::iterator it;

		for (it = memory.begin(); it != memory.end(); it++) {
			cout << left << setw(5) << it->first    // string (key)
				<< ": "
				<< setw(12) << it->second;  // string's value 
			if (line == 3)
				cout << endl;
			line = (line + 1) % 4;
		}
		if (line != 3) cout << endl;
		cout << endl;
	} else {
		bitset<32> x;
		int line = 0;
		for (int i = 0; i < 32; i++) {
			x = registers[i];
			cout << left << "x" << setw(2) << i << ": "
				<< setw(35) << x;
			if (line == 1)
				cout << endl;
			line = (line + 1) % 2;
		}
		cout << endl;
		cout << "printing memory: " << endl;
		map<unsigned int, int>::iterator it;
		line = 0;
		for (it = memory.begin(); it != memory.end(); it++) {
			x = it->second;
			cout << left << setw(6) << it->first                // address
				<< ": "
				<< left << setw(35) << x;			 // value 
			if (line == 1)
				cout << endl;
			line = (line + 1) % 2;
		}
		if (line != 1) cout << endl;
		cout << endl;
	}
}



void readInstruction(string tempS) {

	string s;

	stringstream ss(tempS);
	ss >> s;
	s = lower(s);
	vector<string> v;


	if (s == "lui") {
		getline(ss, s);
		v = UJtype(s); // v[0] = rd, v[1] = imm (as string)
		lui(map_reg(v[0]), stoi(v[1]));
		PC += 4;
	} else if (s == "auipc") {
		getline(ss, s);
		v = UJtype(s); // v[0] = rd, v[1] = imm (as string)
		auipc(map_reg(v[0]), stoi(v[1]));
		PC += 4;
	} else if (s == "jal") {
		getline(ss, s);
		v = UJtype(s); // v[0] = rd, v[1] = label (as string)
		jal(map_reg(v[0]), v[1]);
	} else if (s == "jalr") {
		getline(ss, s);
		v = Itype(s); // v[0] = rd, v[1] = rs, v[2] = imm
		jalr(map_reg(v[0]), map_reg(v[1]), stoi(v[2]));
	} else if (s == "beq") {
		getline(ss, s);
		v = Stype(s); //v[0] = rs1, v[1]=rs2, v[2] = LABEL
		if (registers[map_reg(v[0])] == registers[map_reg(v[1])]) {
			PC = labels[v[2]];
		} else {
			PC += 4;
		}

	} else if (s == "bne") {
		getline(ss, s);
		v = Stype(s); //v[0] = rs1, v[1]=rs2, v[2] = LABEL

		if (registers[map_reg(v[0])] != registers[map_reg(v[1])]) {
			PC = labels[v[2]];
		} else {
			PC += 4;
		}

	} else if (s == "blt") {
		getline(ss, s);
		v = Stype(s); //v[0] = rs1, v[1]=rs2, v[2] = LABEL

		if (registers[map_reg(v[0])] < registers[map_reg(v[1])]) {
			PC = labels[v[2]];
		} else {
			PC += 4;
		}

	} else if (s == "bge") {
		getline(ss, s);
		v = Stype(s); //v[0] = rs1, v[1]=rs2, v[2] = LABEL

		if (registers[map_reg(v[0])] >= registers[map_reg(v[1])]) {
			PC = labels[v[2]];
		} else {
			PC += 4;
		}

	} else if (s == "bltu") {
		getline(ss, s);
		v = Stype(s); //v[0] = rs1, v[1]=rs2, v[2] = LABEL

		if (((unsigned int)registers[map_reg(v[0])]) < ((unsigned int)registers[map_reg(v[1])])) {
			PC = labels[v[2]];
		} else {
			PC += 4;
		}

	} else if (s == "bgeu") {
		getline(ss, s);
		v = Stype(s); //v[0] = rs1, v[1]=rs2, v[2] = LABEL

		if (((unsigned int)registers[map_reg(v[0])]) >= ((unsigned int)registers[map_reg(v[1])])) {
			PC = labels[v[2]];
		} else {
			PC += 4;
		}

	}
	// edited
	else if (s == "lb") {
		getline(ss, s);
		v = Itype(s); //v[0] = rd, v[1]=rs1, v[2]=imm

		//don't change the value of rd if its x0
		if (map_reg(v[0]) != 0) {
			int temp = stoi(v[2]) + registers[map_reg(v[1])];	//address = immidiate + value stored in rs1

			//find closest multiple of four
			int r = temp % 4;	//remainder
			temp = temp - r;

			//we load the entire word
			if (memory.find(temp) != memory.end()) {
				//The address already exists and has a value given by the user.
				temp = memory[temp];
			} else {	//the address doesn't exist in our map.
				temp = 0; //We will assume all values in the memory is by default equal to 0.
			}

			//we get the byte we want from the word
			temp = temp << (8 * (3 - r));
			temp = temp >> 24;

			//incase r=0,  << 24 and >> 24
			//incase r=1,  << 16 and >> 24
			//incase r=2,  << 8 and >> 24
			//incase r=3,  << 0 and >> 24
			
			registers[map_reg(v[0])] = temp;
		}

		PC += 4;


	} else if (s == "lh") {
		getline(ss, s);
		v = Itype(s); //v[0] = rd, v[1]=rs1, v[2]=imm

		//When it comes to loading halfwords, we must make sure that the address is a multiple of 2, this is the same functionality in RARS simulator.
		//We will take this as an assumption, that any use of lw, lh, sw, sh is correct. So, no need to check

		//don't change the value of rd if its x0
		if (map_reg(v[0]) != 0) {
			int wordAdd = stoi(v[2]) + registers[map_reg(v[1])];	//address = immidiate + value stored in rs1

			//find closest multiple of four (closest word in memory)
			int r = wordAdd % 4;	//remainder
			wordAdd = wordAdd - r;
			int temp;

			if (memory.find(wordAdd) != memory.end()) {
				//The address already exists and has a value given by the user.
				temp = memory[wordAdd];
			} else {	//the address doesn't exist in our map.
				temp = 0; //We will assume all values in the memory is by default equal to 0.
			}

			temp = temp << (8 * (2 - r));
			temp = temp >> 16;


			registers[map_reg(v[0])] = temp;

					
		}

		PC += 4;

	} else if (s == "lw") {
		getline(ss, s);
		v = Itype(s); //v[0] = rd, v[1]=rs1, v[2]=imm

		//When it comes to loading halfwords, we must make sure that the address is a multiple of 2, this is the same functionality in RARS simulator.
		//We will take this as an assumption, that any use of lw, lh, sw, sh is correct. So, no need to check.

		//don't change the value of rd if its x0
		if (map_reg(v[0]) != 0) {
			//we will make the value of register rd = to the value stored in the address of immidiate + value stored in rs1
			int temp = stoi(v[2]) + registers[map_reg(v[1])];	//address = immidiate + value stored in rs1

			if (memory.find(temp) != memory.end()) {
				//The address already exists and has a value given by the user.
				temp = memory[temp];
			} else {	//the address doesn't exist in our map.
				temp = 0; //We will assume all values in the memory is by default equal to 0.
			}

			registers[map_reg(v[0])] = temp;
		}

		PC += 4;

	} else if (s == "lbu") {
		getline(ss, s);
		v = Itype(s); //v[0] = rd, v[1]=rs1, v[2]=imm




		//don't change the value of rd if its x0
		if (map_reg(v[0]) != 0) {
			int temp = stoi(v[2]) + registers[map_reg(v[1])];	//address = immidiate + value stored in rs1

			//find closest multiple of four
			int r = temp % 4;	//remainder
			temp = temp - r;

			//we load the entire word
			if (memory.find(temp) != memory.end()) {
				//The address already exists and has a value given by the user.
				temp = memory[temp];
			} else {	//the address doesn't exist in our map.
				temp = 0; //We will assume all values in the memory is by default equal to 0.
			}

			//we get the byte we want from the word
			unsigned int tempTwo = temp << (8 * (3 - r));
			tempTwo = tempTwo >> 24;

			registers[map_reg(v[0])] = tempTwo;
		}

		PC += 4;

	} else if (s == "lhu") {
		getline(ss, s);
		v = Itype(s); //v[0] = rd, v[1]=rs1, v[2]=imm


		//When it comes to loading halfwords, we must make sure that the address is a multiple of 2, this is the same functionality in RARS simulator.
		//We will take this as an assumption, that any use of lw, lh, sw, sh is correct. So, no need to check

		//don't change the value of rd if its x0
		if (map_reg(v[0]) != 0) {
			int wordAdd = stoi(v[2]) + registers[map_reg(v[1])];	//address = immidiate + value stored in rs1

			//find closest multiple of four (closest word in memory)
			int r = wordAdd % 4;	//remainder
			wordAdd = wordAdd - r;
			int temp;

			if (memory.find(wordAdd) != memory.end()) {
				//The address already exists and has a value given by the user.
				temp = memory[wordAdd];
			} else {	//the address doesn't exist in our map.
				temp = 0; //We will assume all values in the memory is by default equal to 0.
			}

			unsigned int tempTwo = temp << (8 * (2 - r));
			tempTwo = tempTwo >> 16;
			registers[map_reg(v[0])] = tempTwo;	
		}

		PC += 4;

	} else if (s == "sb") {
		getline(ss, s);
		v = Btype(s); //v[0] = rs1, v[1]=immidate, v[2]=rs2

		int temp = stoi(v[1]) + registers[map_reg(v[0])];	//address = immidiate + value stored in rs1

		//find closest word address
		int r = temp % 4;	//remainder
		temp = temp - r;

		if (memory.find(temp) == memory.end()) {
			//that means the address doesn't exist in our map, so we add it to the map with a default value of 0.
			memory.insert(pair<int, int>(temp, 0));
		}

		//get the byte
		unsigned int theByte = registers[map_reg(v[2])] << 24;
		theByte = theByte >> 24;
		theByte = theByte << 8 * (r);

		//insert the byte into the apt position in the word without changing the rest of the word values.
		int tempWordOne = memory[temp];
		unsigned int tempWordTwo = memory[temp];

		//appearently you can't shift by 32 bits in C.

		if (r==3) {
			tempWordOne = 0;
		} else {
			tempWordOne = tempWordOne >> (8 * (r + 1));
			tempWordOne = tempWordOne << (8 * (r + 1));
		}
		

		if (r==0) {
			tempWordTwo = 0;
		} else {
			tempWordTwo = tempWordTwo << (8 * (4 - r));
			tempWordTwo = tempWordTwo >> (8 * (4 - r));
		}

		memory[temp] = tempWordOne | tempWordTwo;
		memory[temp] = memory[temp] | theByte;

	

		memory[temp] = memory[temp] | theByte;


		PC += 4;

	} else if (s == "sh") {
		getline(ss, s);
		v = Btype(s); //v[0] = rs1, v[1]=immidate, v[2]=rs2

		int temp = stoi(v[1]) + registers[map_reg(v[0])];	//address = immidiate + value stored in rs1

		//find closest word address, we will assume r is either 0 or 2 because we are dealing with halfwords.
		int r = temp % 4;	//remainder
		temp = temp - r;

		if (memory.find(temp) == memory.end()) {
			//that means the address doesn't exist in our map, so we add it to the map with a default value of 0.
			memory.insert(pair<int, int>(temp, 0));
		}

		//get the byte
		unsigned int theByte = registers[map_reg(v[2])] << 16;
		theByte = theByte >> 16;
		theByte = theByte << 8 * (r);	//r is either 0 or 2.
		
		//insert the byte into the apt position in the word without changing the rest of the word values.
		unsigned int tempWordOne = memory[temp];
		if (r==0) {
			tempWordOne = tempWordOne >> 16;
			tempWordOne = tempWordOne << 16;
		} else {
			tempWordOne = tempWordOne << 16;
			tempWordOne = tempWordOne >> 16;
		}
	
		
		memory[temp] = tempWordOne | theByte;

		PC += 4;

	} else if (s == "sw") {
		getline(ss, s);
		v = Btype(s); //v[0] = rs1, v[1]=immidate, v[2]=rs2

		//we will make the value of address (immidiate + rs1) = value at rs1
		int temp = stoi(v[1]) + registers[map_reg(v[0])];	//address = immidiate + value stored in rs1
		if (memory.find(temp) == memory.end()) {
			//that means the address doesn't exist in our map, so we add it to the map with a default value of 0.
			memory.insert(pair<int, int>(temp, 0));
		}

		memory[temp] = registers[map_reg(v[2])];

		PC += 4;

	} else if (s == "addi") {
		getline(ss, s);
		v = Itype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > imm
		addi(map_reg(v[0]), map_reg(v[1]), stoi(v[2]));
		PC += 4;
	} else if (s == "slti") {

		getline(ss, s);
		v = Itype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > imm
		slti(map_reg(v[0]), map_reg(v[1]), stoi(v[2]));
		PC += 4;

	} else if (s == "sltiu") {

		getline(ss, s);
		v = Itype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > imm
		sltiu(map_reg(v[0]), map_reg(v[1]), stoi(v[2]));
		PC += 4;


	} else if (s == "xori") {

		getline(ss, s);
		v = Itype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > imm
		xori(map_reg(v[0]), map_reg(v[1]), stoi(v[2]));
		PC += 4;

	} else if (s == "ori") {

		getline(ss, s);
		v = Itype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > imm
		ori(map_reg(v[0]), map_reg(v[1]), stoi(v[2]));
		PC += 4;

	} else if (s == "andi") {

		getline(ss, s);
		v = Itype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > imm
		andi(map_reg(v[0]), map_reg(v[1]), stoi(v[2]));
		PC += 4;

	} else if (s == "slli") {

		getline(ss, s);
		v = Itype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > imm
		slli(map_reg(v[0]), map_reg(v[1]), stoi(v[2]));
		PC += 4;

	} else if (s == "srli") {

		getline(ss, s);
		v = Itype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > imm
		srli(map_reg(v[0]), map_reg(v[1]), stoi(v[2]));
		PC += 4;

	} else if (s == "srai") {

		getline(ss, s);
		v = Itype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > imm
		srai(map_reg(v[0]), map_reg(v[1]), stoi(v[2]));
		PC += 4;

	} else if (s == "add") {

		getline(ss, s);
		v = Rtype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > rs2
		add(map_reg(v[0]), map_reg(v[1]), map_reg(v[2]));
		PC += 4;

	} else if (s == "sub") {

		getline(ss, s);
		v = Rtype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > rs2
		sub(map_reg(v[0]), map_reg(v[1]), map_reg(v[2]));
		PC += 4;

	} else if (s == "sll") {

		getline(ss, s);
		v = Rtype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > rs2
		sll(map_reg(v[0]), map_reg(v[1]), map_reg(v[2]));
		PC += 4;

	} else if (s == "slt") {

		getline(ss, s);
		v = Rtype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > rs2
		slt(map_reg(v[0]), map_reg(v[1]), map_reg(v[2]));
		PC += 4;

	} else if (s == "sltu") {

		getline(ss, s);
		v = Rtype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > rs2
		sltu(map_reg(v[0]), map_reg(v[1]), map_reg(v[2]));
		PC += 4;

	} else if (s == "xor") {

		getline(ss, s);
		v = Rtype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > rs2
		xor_(map_reg(v[0]), map_reg(v[1]), map_reg(v[2]));
		PC += 4;

	} else if (s == "srl") {

		getline(ss, s);
		v = Rtype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > rs2
		srl(map_reg(v[0]), map_reg(v[1]), map_reg(v[2]));
		PC += 4;

	} else if (s == "sra") {

		getline(ss, s);
		v = Rtype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > rs2
		sra(map_reg(v[0]), map_reg(v[1]), map_reg(v[2]));
		PC += 4;

	} else if (s == "or") {

		getline(ss, s);
		v = Rtype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > rs2
		or_(map_reg(v[0]), map_reg(v[1]), map_reg(v[2]));
		PC += 4;

	} else if (s == "and") {

		getline(ss, s);
		v = Rtype(s); //vector[0]->rd, vector[1]->rs1, vector[2] -- > rs2
		and_(map_reg(v[0]), map_reg(v[1]), map_reg(v[2]));
		PC += 4;

	}
}



// input: known reg such as "x4", "gp"
// output: number of register "4", "3"
int map_reg(string s) {
	if (s == "zero" || s == "x0") return 0;
	if (s == "ra" || s == "x1") return 1;
	if (s == "sp" || s == "x2") return 2;
	if (s == "gp" || s == "x3") return 3;
	if (s == "tp" || s == "x4") return 4;
	if (s == "t0" || s == "x5") return 5;
	if (s == "t1" || s == "x6") return 6;
	if (s == "t2" || s == "x7") return 7;
	if (s == "s0" || s == "x8") return 8;
	if (s == "s1" || s == "x9") return 9;
	if (s == "a0" || s == "x10") return 10;
	if (s == "a1" || s == "x11") return 11;
	if (s == "a2" || s == "x12") return 12;
	if (s == "a3" || s == "x13") return 13;
	if (s == "a4" || s == "x14") return 14;
	if (s == "a5" || s == "x15") return 15;
	if (s == "a6" || s == "x16") return 16;
	if (s == "a7" || s == "x17") return 17;
	if (s == "s2" || s == "x18") return 18;
	if (s == "s3" || s == "x19") return 19;
	if (s == "s4" || s == "x20") return 20;
	if (s == "s5" || s == "x21") return 21;
	if (s == "s6" || s == "x22") return 22;
	if (s == "s7" || s == "x23") return 23;
	if (s == "s8" || s == "x24") return 24;
	if (s == "s9" || s == "x25") return 25;
	if (s == "s10" || s == "x26") return 26;
	if (s == "s11" || s == "x27") return 27;
	if (s == "t3" || s == "x28") return 28;
	if (s == "t4" || s == "x29") return 29;
	if (s == "t5" || s == "x30") return 30;
	if (s == "t6" || s == "x31") return 31;
}




// input: a string of type R without the name of instruction ("s2, s1, s0;")
// returns: a vector containing the registers in the instruction (["s2", "s1", "s0"])
//        : vector[0] -> rd, vector[1] -> rs1, vector[2] --> rs2
vector<string> Rtype(string s) {
	vector<string> parts;
	int n = s.size();
	bool new_p = true;
	string x = "";
	for (int i = 0; i < n; i++) {
		if (isalnum(s[i])) {
			x.push_back(s[i]);
			new_p = false;
		} else {
			if (!new_p) {
				parts.push_back(x);
				x.clear();
				new_p = true;
			}
		}
	}
	if (!new_p)
		parts.push_back(x);
	return parts;
}

// input: a string of type I without the name of instruction such as "s2, s1, 5;" or "x3, 4(x2);"
// returns: a vector containing the registers and imm in the instruction (["s2", "s1", "5"])
//        : vector[0] -> rd, vector[1] -> rs1, vector[2] --> imm
vector<string> Itype(string s) {
	vector<string> parts;
	int n = s.size();
	bool new_p = true;
	string x = "";
	for (int i = 0; i < n; i++) {
		if (isalnum(s[i]) || s[i] == '-') {
			x.push_back(s[i]);
			new_p = false;
		} else {
			if (!new_p) {
				parts.push_back(x);
				x.clear();
				new_p = true;
			}
		}
	}
	if (isdigit(parts[1][0]) || parts[1][0] == '-')
		swap(parts[1], parts[2]);
	if (!new_p)
		parts.push_back(x);
	return parts;
}

// input: a string of type S without the name of instruction such as "rs1, rs2, LABEL;" 
// returns: a vector containing the registers and Label in the instruction (["rs1", "rs2", "LABEL"])
//        : vector[0] -> rs1, vector[1] -> rs2, vector[2] --> LABEL
vector<string> Stype(string s) {
	vector<string> parts;
	int n = s.size();
	bool new_p = true;
	string x = "";
	for (int i = 0; i < n; i++) {
		if (isalnum(s[i])) {
			x.push_back(s[i]);
			new_p = false;
		} else {
			if (!new_p) {
				parts.push_back(x);
				x.clear();
				new_p = true;
			}
		}
	}
	if (!new_p)
		parts.push_back(x);
	return parts;
}



// input: a string of type U or J without the name of instruction such as "rd, imm" 
// returns: a vector containing the registers and immidiate in the instruction (["rd", "imm"])
//        : vector[0] -> rd, vector[1] -> immidiate or label
vector<string> UJtype(string s) {
	vector<string> parts;
	int n = s.size();
	bool new_p = true;
	string x = "";
	for (int i = 0; i < n; i++) {
		if (isalnum(s[i])) {
			x.push_back(s[i]);
			new_p = false;
		} else {
			if (!new_p) {
				parts.push_back(x);
				x.clear();
				new_p = true;
			}
		}
	}
	if (!new_p)
		parts.push_back(x);

	return parts;
}




// input: a string of type B without the name of instruction such as "rs2, imm(rs1);" 
// returns: a vector containing the registers and immidiate in the instruction (["rs1", "immidiate", "rs2"])
//        : vector[0] -> rs1, vector[1] -> immidiate, vector[2] --> rs2
vector<string> Btype(string s) {
	vector<string> parts;
	int n = s.size();
	bool new_p = true;
	string x = "";
	for (int i = 0; i < n; i++) {
		if (isalnum(s[i])) {
			x.push_back(s[i]);
			new_p = false;
		} else {
			if (!new_p) {
				parts.push_back(x);
				x.clear();
				new_p = true;
			}
		}
	}
	if (!new_p)
		parts.push_back(x);

	//here we will have part[0]=rs2, part[1]=immidiate, part[2]=rs1, 
	//we will swap 0 and 2.
	swap(parts[0], parts[2]);

	return parts;
}



bool get_label_instrct(string file_name) {
	ifstream inst(file_name);
	if (!inst.is_open()) {
		cout << "File couldn't be found please check again.\n";
		return false;
	}
	string line;

	bool done;
	int cntr = 0;
	while (inst) {
		getline(inst, line);
		if (!line.empty()) {
			done = false;
			for (int i = 0; i < line.size(); i++) {
				// if we have a label
				if (line[i] == ':') {
					done = true;
					labels.insert({ trim(line.substr(0, i)), PC + cntr * 4 });
					if (trim(line).size() - i > 1) {
						instructs.insert({ PC + cntr * 4, trim(line.substr(i + 1, line.size() - i + 1)) });
						cntr++;
					}
				}
			}
			if (!done) {
				instructs.insert({ PC + cntr * 4, trim(line) });
				cntr++;
			}
		}
	}

	inst.close();
	return true;
}

void lui(int rd, int imm) {
	if (rd != 0) {
		registers[rd] = (imm << 12);
	}
}

void jal(int rd, string label) {
	if (rd != 0) {
		registers[rd] = PC + 4;
	}
	PC = labels[label];
}

void jalr(int rd, int base, int offset) {
	if (rd != 0) {
		registers[rd] = PC + 4;
	}
	PC = registers[base] + offset;
}

void auipc(int rd, int imm) {
	if (rd != 0) {
		registers[rd] = PC + (imm << 12);
	}
}

string ltrim(const string& s) {
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(const string& s) {
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string& s) {
	return rtrim(ltrim(s));
}


void add(int x, int y, int z) {
	if (x == 0)
		return;
	else {
		registers[x] = registers[y] + registers[z];
	}
}

void sub(int x, int y, int z) {
	if (x == 0)
		return;
	else {
		registers[x] = registers[y] - registers[z];
	}
}

void sll(int x, int y, int z) {
	if (x == 0)
		return;
	else {
		registers[x] = registers[y] << (registers[z] & 31);
	}
}

void srl(int x, int y, int z)// unsigned only
{
	if (x == 0)
		return;
	else if (registers[y] < 0) {
		unsigned int  temp = registers[y];

		registers[x] = temp >> (registers[z] & 31);
	} else
		registers[x] = registers[y] >> (registers[z] & 31);
}

void sra(int x, int y, int z) {
	if (x == 0)
		return;
	else if (registers[y] < 0) {
		unsigned int  temp = registers[y];
		registers[x] = temp >> (registers[z] & 31);
	} else
		registers[x] = registers[y] >> (registers[z] & 31);
}

void slt(int x, int y, int z) {
	if (x == 0)
		return;
	else if (registers[y] < registers[z]) {
		registers[x] = 1;
	} else
		registers[x] = 0;
}



void sltu(int x, int y, int z) {
	if (x == 0)
		return;
	else if (((unsigned int)registers[y]) < ((unsigned int)registers[z])) {
		registers[x] = 1;
	} else
		registers[x] = 0;
}

void and_(int x, int y, int z) {
	if (x == 0)
		return;
	else
		registers[x] = registers[y] & registers[z];
}

void or_(int x, int y, int z) {
	if (x == 0)
		return;
	else
		registers[x] = registers[y] | registers[z];
}

void xor_(int x, int y, int z) {
	if (x == 0)
		return;
	else
		registers[x] = registers[y] ^ registers[z];
}


/////////////////////////////////////////////////////////Itype

void addi(int x, int y, int z) {
	if (x == 0)
		return;
	else {
		registers[x] = registers[y] + z;
	}
}

void slti(int x, int y, int z) {
	if (x == 0)
		return;
	else if (registers[y] < z) {
		registers[x] = 1;

	} else
		registers[x] = 0;
}

void sltiu(int x, int y, int z) {
	if (x == 0)
		return;
	else if (((unsigned int)registers[y]) < ((unsigned int)z)) {
		registers[x] = 1;

	} else
		registers[x] = 0;
}

void xori(int x, int y, int z) {
	if (x == 0)
		return;
	else
		registers[x] = registers[y] ^ z;
}

void andi(int x, int y, int z) {
	if (x == 0)
		return;
	else
		registers[x] = registers[y] & z;
}

void ori(int x, int y, int z) {
	if (x == 0)
		return;
	else
		registers[x] = registers[y] | z;
}

void slli(int x, int y, int z) {
	if (x == 0)
		return;
	else
		registers[x] = registers[y] << (z & 31);
}

void srli(int x, int y, int z) {
	if (x == 0)
		return;
	else if (registers[y] < 0) {
		unsigned int  temp = registers[y];
		registers[x] = temp >> (z & 31);
	} else
		registers[x] = registers[y] >> (z & 31);
}

void srai(int x, int y, int z) {
	if (x == 0)
		return;
	else if (registers[y] < 0) {
		unsigned int  temp = registers[y];
		registers[x] = temp >> (z & 31);
	} else
		registers[x] = registers[y] >> (z & 31);
}

string lower(string s) {
	string output;
	for (int i = 0; i < s.size(); i++) {
		output.push_back(tolower(s.at(i)));
	}
	return output;
}

bool validpc(string s) {
	for (int i = 0; i < s.size(); i++) {
		if (!isdigit(s[i]))
			return false;
	}
	if (s.size() > 10) {
		return false;
	}
	long long temp = stoll(s);
	if (temp > 4294967295 || temp < 0)
		return false;
	return true;
}