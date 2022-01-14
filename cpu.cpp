// Aluno: Yure Samarone Gomes Duarte
// BCC 2019 - UFOPA
#include <iostream>
#include <cmath>
using namespace std;
const int word_size = 8;
const unsigned int array_memory_size = word_size * 2;

//Instruções
const char ADD = 'A';
const char HALT = 'H';
const char LOAD = 'L';
const char OUT = 'O';
const char READ = 'R';
const char WRITE = 'W';

// Registradores
const short REG0 = 0;
const short REG1 = 1;
const short REG2 = 2;
const short REG3 = 3;

void two_complement(bool *v, int word_size);
void two_complement(bool *v);
void to_bits(int a, bool *v, int len);
int to_int(bool v[]);
void print(bool[]);
void cpy(bool*, bool*, int);

class Data
{
public:
    Data(){};
    virtual ~Data(){};
};

class Memory
{
public:
    Memory()
    {
        for (unsigned int i = 0; i < array_memory_size; i++)
        {
            elements[i] = new Data();
        }
    }
    Data *access(const char type, unsigned int adress)
    {
        if (type == READ && adress < array_memory_size)
        {
            return elements[adress];
        }
        return 0;
    }
    void access(const char type, unsigned int adress, Data *data)
    {
        if (type == WRITE && adress < array_memory_size)
        {
            elements[adress] = data;
        }
    }

private:
    Data *elements[array_memory_size];
};

class IntegerNumber : public Data
{
public:
    IntegerNumber() : Data()
    {
        number_bits = new bool[word_size];
        for(int i = 0; i< word_size; i++){
            number_bits[i] = false;
        }
    }
    IntegerNumber(int num) : Data()
    {
        number_bits = new bool[word_size];
        to_bits(num, number_bits, word_size);
    }
    bool *number_bits;
};

class Instruction : public Data
{
public:
    Instruction() : Data() {}
    Instruction(char opCode, unsigned int ref_element_a, unsigned int ref_element_b) : Data()
    {
        this->opCode = opCode;
        this->ref_element_a = ref_element_a;
        this->ref_element_b = ref_element_b;
    }

    char opCode;
    unsigned int ref_element_a;
    unsigned int ref_element_b;
};

class MAR
{
public:
    MAR(unsigned int adress, const char type)
    {
        this->adress = adress;
        this->type = type;
    }
    MAR() {}
    unsigned int adress;
    char type;
};
class Registers
{

public:
    bool *reg0;
    bool *reg1;
    bool *reg2;
    bool *reg3;
    Registers()
    {
        reg0 = new bool[word_size];
        reg1 = new bool[word_size];
        reg2 = new bool[word_size];
        reg3 = new bool[word_size];
        for(int i = 0; i < word_size; i++){
            reg0[i] = false;
            reg1[i] = false;
            reg2[i] = false;
            reg3[i] = false;
        }
    }
};

class MBR
{
public:
    MBR(Memory *m, MAR *mar)
    {

        this->mar = mar;
        this->mem_ref = m;
    }
    MBR() {}
    void set_mem_ref(Memory *m)
    {
        this->mem_ref = m;
    }
    void set_mar(MAR *mar)
    {
        this->mar = mar;
    }
    Data *getData()
    {
        return d;
    }
    void get_element_in_MAR()
    {
        d = mem_ref->access(mar->type, mar->adress);
    }

private:
    Memory *mem_ref;
    MAR *mar;
    Data *d;
};
class ULA
{

private:
    void _add(bool a[], bool b[], int word_size, bool *f)
    {
        this->overflow = false;
        bool sum = false;
        bool carry = false;

        for (int i = word_size - 1; i >= 0; i--)
        {
            sum = carry ^ (a[i] ^ b[i]);
            carry = (a[i] & b[i]) | (carry & (a[i] ^ b[i]));
            f[i] = sum;
        }
        if ( (to_int(a) > 0) && (to_int(b) > 0)  == (to_int(f)  < 0 ) || to_int(f) == -128 )
        { // regra do overflow
            this->overflow = true;
        } else if ( (to_int(a) < 0) && (to_int(b) < 0)  == (to_int(f)  > 0) || to_int(f) == -128){
            this->overflow = true;
        }
    }

public:
    bool overflow;
    void add(bool op_a[], bool op_b[], bool *res, int tam)
    {
        this->_add(op_a, op_b, tam, res);
    }
    ULA()
    {
    }
};

class UC
{
public:
    UC() {
        halt  = false;
    }
    void increment_program_counter(unsigned int &program_counter)
    {
        cout << "program counter: " << program_counter << endl;
        program_counter++;
    }
    void get_next_instruction(MAR *mar, unsigned int &program_counter, MBR *mbr)
    {
        mar->adress = program_counter;
        mar->type = READ;
        mbr->get_element_in_MAR();
    }
    void get_instruction_from_MBR(MBR *mbr)
    {
        if (Instruction *temp = dynamic_cast<Instruction *>(mbr->getData()))
        {
            cout << "the instruction was retrieved successfully and is loading in the Instruction Register" << endl;
            instructionRegister = temp;
        }
    }
    void interpret_and_execute_instruction(ULA *ula, Registers *registers, MAR *mar, MBR *mbr)
    {
        switch (instructionRegister->opCode)
        {
        case ADD:
        {
            if (instructionRegister->ref_element_a == REG0)
            {
                switch (instructionRegister->ref_element_b)
                {
                case REG1:
                    ula->add(registers->reg0, registers->reg1, registers->reg0, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;
                case REG2:
                    ula->add(registers->reg0, registers->reg2, registers->reg2, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;
                case REG3:
                    ula->add(registers->reg0, registers->reg3, registers->reg0, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;

                default:
                    break;
                }
            }
            else if (instructionRegister->ref_element_a == REG1)
            {
                switch (instructionRegister->ref_element_b)
                {
                case REG2:
                    ula->add(registers->reg1, registers->reg2, registers->reg1, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;
                case REG3:
                    ula->add(registers->reg1, registers->reg3, registers->reg1, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;
                case REG0:
                    ula->add(registers->reg1, registers->reg0, registers->reg1, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;

                default:
                    break;
                }
            }
            else if (instructionRegister->ref_element_a == REG2)
            {
                switch (instructionRegister->ref_element_b)
                {
                case REG0:
                    ula->add(registers->reg2, registers->reg0, registers->reg2, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;
                case REG1:
                    ula->add(registers->reg1, registers->reg3, registers->reg1, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;
                case REG3:
                    ula->add(registers->reg1, registers->reg0, registers->reg1, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;

                default:
                    break;
                }
            }
            else if (instructionRegister->ref_element_a == REG3)
            {
                switch (instructionRegister->ref_element_b)
                {
                case REG0:
                    ula->add(registers->reg3, registers->reg0, registers->reg3, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;
                case REG1:
                    ula->add(registers->reg3, registers->reg1, registers->reg3, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;
                case REG2:
                    ula->add(registers->reg3, registers->reg2, registers->reg3, word_size);
                    flags.overflow = ula->overflow;
                    cout << (flags.overflow ? "Overflow: true" : "") << endl;
                    break;

                default:
                    break;
                }
            }
        }
        break;
        case LOAD:
        {
            if (instructionRegister->ref_element_a == REG0)
            {
                mar->adress = instructionRegister->ref_element_b;
                mar->type = READ;
                mbr->get_element_in_MAR();
                if (IntegerNumber *number = dynamic_cast<IntegerNumber *>(mbr->getData()))
                {
                    cpy(number->number_bits, registers->reg0, word_size);
                }
            }
            else if (instructionRegister->ref_element_a == REG1)
            {
                mar->adress = instructionRegister->ref_element_b;
                mar->type = READ;
                mbr->get_element_in_MAR();
                if (IntegerNumber *number = dynamic_cast<IntegerNumber *>(mbr->getData()))
                {
                    cpy(number->number_bits, registers->reg1, word_size);
                }
            }
            else if (instructionRegister->ref_element_a == REG2)
            {
                mar->type = READ;
                mar->adress = instructionRegister->ref_element_b;
                mbr->get_element_in_MAR();
                if (IntegerNumber *number = dynamic_cast<IntegerNumber *>(mbr->getData()))
                {
                    cpy(number->number_bits, registers->reg2, word_size);
                }
            }
            else if (instructionRegister->ref_element_a == REG3)
            {
                mar->adress = instructionRegister->ref_element_b;
                mar->type = READ;
                mbr->get_element_in_MAR();
                if (IntegerNumber *number = dynamic_cast<IntegerNumber *>(mbr->getData()))
                {
                    cpy(number->number_bits, registers->reg3, word_size);
                }
            }
        }
        break;
        case HALT:
        {
            this->halt = true;
        }
        break;
        case OUT:
        {
            if (instructionRegister->ref_element_a == REG0)
            {
                cout << "OUTPUT: " << to_int(registers->reg0) << endl;
            }
            else if (instructionRegister->ref_element_a == REG1)
            {
                cout << "OUTPUT: " << to_int(registers->reg1) << endl;
            }
            else if (instructionRegister->ref_element_a == REG2)
            {
                cout << "OUTPUT: " << to_int(registers->reg2) << endl;
            }
            else if (instructionRegister->ref_element_a == REG3)
            {
                cout << "OUTPUT: " << to_int(registers->reg3) << endl;
            }
        }
        break;
        }
    }

    bool halt;;

private:
    Instruction *instructionRegister;
    struct Flags {
        bool overflow;
    } flags;
};

class CPU
{
public:
    CPU(Memory *memory)
    {
        ula = new ULA();
        program_counter = 0;
        this->memory = memory;
        mar = new MAR();
        registers = new Registers();
        mbr = new MBR(memory, mar);
        uc = new UC();
    };
    void run(){
        unsigned int i = 0;
        while(!uc->halt && i <= array_memory_size){
            uc->get_next_instruction(mar, program_counter, mbr);
            uc->increment_program_counter(program_counter);
            uc->get_instruction_from_MBR(mbr);
            uc->interpret_and_execute_instruction(ula,registers, mar, mbr);
            i++;
        }
        cout << "Executed";
    }

private:
    ULA *ula;
    Registers *registers;
    MAR *mar;
    UC *uc;
    MBR *mbr;
    Memory *memory;
    unsigned int program_counter;
};

int main()
{
    Instruction *i = new Instruction();
    Instruction *i2 = new Instruction();
    Instruction *i3 = new Instruction();
    Instruction *i4 = new Instruction();
    Instruction *i5 = new Instruction();
    i->opCode = LOAD;
    i->ref_element_a = REG0;
    i->ref_element_b = 6;

    i2->opCode = LOAD;
    i2->ref_element_a = REG1;
    i2->ref_element_b = 7;

    i3->opCode = ADD;
    i3->ref_element_a = REG0;
    i3->ref_element_b = REG1;

    i4->opCode = OUT;
    i4->ref_element_a = REG0;

    i5->opCode = HALT;
    Memory *m = new Memory();
    m->access(WRITE, 0, i);
    m->access(WRITE, 1, i2);
    m->access(WRITE, 2, i3);
    m->access(WRITE, 3, i4);
    m->access(WRITE, 4, i5);

    m->access(WRITE, 6, new IntegerNumber(2));
    m->access(WRITE, 7, new IntegerNumber(2));
    CPU *cpu = new CPU(m);
    cpu->run();
}
/* Funções de Apoio */ 

void two_complement(bool *v, int word_size)
{
    for (int i = 0; i < word_size; i++)
    {
        v[i] = !v[i];
    }
    for (int i = word_size - 1; i >= 0; i--)
    {
        if (v[i] == false)
        {
            v[i] = true;
            break;
        }
        else if (v[i] == true)
        {
            v[i] = false;
        }
    }
}
void two_complement(bool *v)
{
    for (int i = 0; i < word_size; i++)
    {
        v[i] = !v[i];
    }
    for (int i = word_size - 1; i >= 0; i--)
    {
        if (v[i] == false)
        {
            v[i] = true;
            break;
        }
        else if (v[i] == true)
        {
            v[i] = false;
        }
    }
}
void to_bits(int a, bool *v, int len)
{
    int aux = a;
    for (int i = len - 1; i >= 0; i--)
    {
        if (aux)
        {
            v[i] = aux % 2;
            aux /= 2;
        }
        else
        {
            v[i] = false;
        }
    }
    if (a < 0)
    {
        two_complement(v, len);
    }
}
int to_int(bool v[])
{
    int res = 0;
    if (v[0] == 1)
    {
        res = pow(2, word_size ) * -1;
    }
    for (int i = word_size - 1; i >= 0; i--)
    { // MSB reservado para bit de sinal
        if (v[i])
            res += pow(2, word_size - 1 - i);
    }
    return res;
}
void print(bool v[])
{
    cout << "print: ";
    for (int i = 0; i < word_size; ++i)
    {
        cout << v[i];
    }
    cout << endl;
}
void cpy(bool *from, bool *to, int tam)
{
    for (int i = 0; i < tam; i++)
    {
        to[i] = from[i];
    }
}
