/*
 * apex_cpu.c
 * Contains APEX cpu pipeline implementation
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"
#include "apex_macros.h"

/* Converts the PC(4000 series) into array index for code memory
 *
 * Note: You are not supposed to edit this function
 */


static int
get_code_memory_index_from_pc(const int pc)
{
    return (pc - 4000) / 4;
}

static void
print_instruction(const CPU_Stage *stage)
{
    switch (stage->opcode)
    {
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_DIV:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_XOR:
        {
            printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
                    stage->rs2);
            break;
        }

        case OPCODE_MOVC:
        {
            printf("%s,R%d,#%d ", stage->opcode_str, stage->rd, stage->imm);
            break;
        }

        case OPCODE_ADDL:
        case OPCODE_SUBL:
        {
            printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
                    stage->imm);
            break;
        }

        case OPCODE_LDR:
        {
            printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
                    stage->imm);
            break;
        }
        
        case OPCODE_STR:
        {
            printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rs1, stage->rs2,
                    stage->rs3);
            break;
        }

        case OPCODE_CMP:
        {
            printf("%s,R%d,R%d ", stage->opcode_str, stage->rs1, stage->rs2);
            break;
        }


        case OPCODE_LOAD:
        {
            printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
                    stage->imm);
            break;
        }

        case OPCODE_STORE:
        {
            printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs1, stage->rs2,
                    stage->imm);
            break;
        }

        case OPCODE_BZ:
        case OPCODE_BNZ:
        {
            printf("%s,#%d ", stage->opcode_str, stage->imm);
            break;
        }

        case OPCODE_HALT:
        {
            printf("%s", stage->opcode_str);
            break;
        }
    }
}

/* Debug function which prints the CPU stage content
 *
 * Note: You can edit this function to print in more detail
 */
static void
print_stage_content(const char *name, const CPU_Stage *stage)
{
    printf("%-15s: pc(%d) ", name, stage->pc);
    print_instruction(stage);
    printf("\n");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
static void
print_reg_file(const APEX_CPU *cpu)
{
    int i;

    printf("----------\n%s\n----------\n", "Registers:");

    for (int i = 0; i < REG_FILE_SIZE / 2; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);

        
    }

    printf("\n");

    for (i = (REG_FILE_SIZE / 2); i < REG_FILE_SIZE; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
        
    }

    printf("\n");
}


int comparator(int a,int b)
{
    if(a == b)
    {
        return TRUE;
    }
    return FALSE;
}


/*
 * Fetch Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_fetch(APEX_CPU *cpu)
{
    APEX_Instruction *current_ins;

    if (cpu->fetch.has_insn)
    {
        /* This fetches new branch target instruction from next cycle */


        if (cpu->fetch_from_next_cycle == TRUE)
        {
            cpu->fetch_from_next_cycle = FALSE;

            /* Skip this cycle*/
            return;
        }

        /* Store current PC in fetch latch */
        cpu->fetch.pc = cpu->pc;

        /* Index into code memory using this pc and copy all instruction fields
        / * into fetch latch  */
        current_ins = &cpu->code_memory[get_code_memory_index_from_pc(cpu->pc)];
        strcpy(cpu->fetch.opcode_str, current_ins->opcode_str);
        cpu->fetch.opcode = current_ins->opcode;
        cpu->fetch.rd = current_ins->rd;
        cpu->fetch.rs1 = current_ins->rs1;
        cpu->fetch.rs2 = current_ins->rs2;
        cpu->fetch.rs3 = current_ins->rs3;
        cpu->fetch.imm = current_ins->imm;
        
        
        /* Update PC for next instruction */
        
        
            cpu->pc += 4;
            /* Copy data from fetch latch to decode latch*/
            cpu->decode = cpu->fetch;
    
        

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Fetch", &cpu->fetch);
        }
        

        if(cpu->fetch.opcode == OPCODE_NOP)
        {
            printf("NOP");
        }
        /* Stop fetching new instructions if HALT is fetched */
        if (cpu->fetch.opcode == OPCODE_HALT )
        {
            //cpu->pc += 4;
            //cpu->decode = cpu->fetch;
            cpu->fetch.has_insn = FALSE;
        }
        
    }
    else
        {
            printf("Fetch : Empty\n");
        }

    //for (int i =0;i<16;i++)
    //{
    //    printf("Register Status: %d \n",cpu->regs_status[i]);
    //}
}

/*
 * Decode Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_decode(APEX_CPU *cpu)
{
    
    if (cpu->decode.has_insn)
    {
        /* Read operands from register file based on the instruction type */
        
        switch (cpu->decode.opcode)
        {
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_MUL:
            case OPCODE_DIV:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_XOR:
            {
                        //printf("%d\t %d\t%d\t%d",cpu->writeback.
                        //result_bus.tag,cpu->writeback.result_bus.buffer,cpu->execute.
                        //result_bus.tag,cpu->execute.result_bus.buffer);
                if(cpu->regs_status[cpu->decode.rs1] == 1 && cpu->regs_status[cpu->decode.rs2] == 1)
                {
                    cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                    cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                    cpu->regs_status[cpu->decode.rd] = 0;
                    cpu->decode.checker = 0;
                }
                else
                {

                    ///For Memory :
                    if(comparator(cpu->decode.rs1,cpu->writeback.result_bus.tag))
                    {

                        ///printf("%d\t %d",cpu->writeback.
                        //result_bus.tag,cpu->writeback.result_bus.buffer);
                        cpu->decode.rs1_value = cpu->writeback.result_bus.buffer;
                        cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                    }

                    else if(comparator(cpu->decode.rs2,cpu->writeback.result_bus.tag))
                    {
                        cpu->decode.rs2_value = cpu->writeback.result_bus.buffer;
                        cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                    }

                //For execute :
                    else if(comparator(cpu->decode.rs1,cpu->execute.result_bus.tag))
                    {
                        cpu->decode.rs1_value = cpu->execute.result_bus.buffer;
                        cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];

                    }

                    else if(comparator(cpu->decode.rs2,cpu->execute.result_bus.tag))
                    {
                        cpu->decode.rs2_value = cpu->execute.result_bus.buffer;
                        cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];

                    }
                    else 
                    {
                        cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                        cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                    }
                    cpu->regs_status[cpu->decode.rd] = 0;
                    cpu->decode.checker = 0;
                }
                break;
            }

            case OPCODE_ADDL:
            case OPCODE_SUBL:
            case OPCODE_LOAD:
            {
                if(cpu->regs_status[cpu->decode.rs1] == 1 )
                {
                    cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                    cpu->regs_status[cpu->decode.rd] = 0;
                    cpu->decode.checker = 0;
                }
                else
                {
                    if(comparator(cpu->decode.rs1,cpu->writeback.result_bus.tag))
                    {
                        cpu->decode.rs1_value = cpu->writeback.result_bus.buffer;
                    }
                    else 
                        cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];

                    if(comparator(cpu->decode.rs1,cpu->execute.result_bus.tag))
                    {
                        cpu->decode.rs1_value = cpu->execute.result_bus.buffer;
                    }
                    else 
                        cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];                   

                    cpu->regs_status[cpu->decode.rd] = 0;
                    cpu->decode.checker = 0;
                
                }
                break;
            }


            case OPCODE_STR:
            {
                if(comparator(cpu->decode.rs1,cpu->memory.rd))
                {
                    cpu->decode.rs1_value = cpu->memory.result_bus.buffer;
                }
                if(comparator(cpu->decode.rs1,cpu->execute.rd))
                {
                    cpu->decode.rs1_value = cpu->execute.result_bus.buffer;
                }
                else
                {
                    cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                }
                ///
                if(comparator(cpu->decode.rs2,cpu->memory.rd))
                {
                    cpu->decode.rs2_value = cpu->memory.result_bus.buffer;
                }
                if(comparator(cpu->decode.rs2,cpu->execute.rd))
                {
                    cpu->decode.rs2_value = cpu->execute.result_bus.buffer;
                }
                else
                {
                    cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                }

                //
                if(comparator(cpu->decode.rs3,cpu->memory.rd))
                {
                    cpu->decode.rs3_value = cpu->memory.result_bus.buffer;
                }
                if(comparator(cpu->decode.rs3,cpu->execute.rd))
                {
                    cpu->decode.rs3_value = cpu->execute.result_bus.buffer;
                }
                else
                {
                    cpu->decode.rs3_value = cpu->regs[cpu->decode.rs3];
                }
                break;
            }
            case OPCODE_LDR:
            case OPCODE_CMP:
            case OPCODE_STORE:
            {
                if(comparator(cpu->decode.rs1,cpu->memory.rd))
                {
                    cpu->decode.rs1_value = cpu->memory.result_bus.buffer;
                }
                if(comparator(cpu->decode.rs1,cpu->execute.rd))
                {
                    cpu->decode.rs1_value = cpu->execute.result_bus.buffer;
                }
                else
                {
                    cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                }

                if(comparator(cpu->decode.rs2,cpu->memory.rd))
                {
                    cpu->decode.rs2_value = cpu->memory.result_bus.buffer;
                }
                if(comparator(cpu->decode.rs2,cpu->execute.rd))
                {
                    cpu->decode.rs2_value = cpu->execute.result_bus.buffer;
                }
                else
                {
                    cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                }
                    //cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                    
                
                
                break;
            }

            case OPCODE_MOVC:
            {
                
                break;
            }
            case OPCODE_HALT:
            {
                /* MOVC doesn't have register operands */
                cpu->execute = cpu->decode;
                //printf("d/rf");
                cpu->decode.has_insn = FALSE;
                break;
            }

            case OPCODE_NOP:
            {
                printf("NOP");
                break;
            }
        }

        /* Copy data from decode latch to execute latch*/

        
        //if (tempo)
        //{
          //  printf("checking validity");
        
        cpu->execute = cpu->decode;
        cpu->decode.has_insn = FALSE;
        cpu->execute.has_insn = TRUE;
        
        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Decode/RF", &cpu->decode);
        }
        
    
    }
    else
        {
            printf("Decode :  Empty\n");
        }
}

/*
 * Execute Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */

static void
APEX_execute(APEX_CPU *cpu)
{
    if (cpu->execute.has_insn)
    {
        /* Execute logic based on instruction type */

        switch (cpu->execute.opcode)
        {
            case OPCODE_ADD:
            {
                cpu->execute.result_bus.buffer 
                    = cpu->execute.rs1_value + cpu->execute.rs2_value ;
                
                cpu->execute.result_bus.tag = cpu->execute.rd;

                
                
                break;
            }

            case OPCODE_ADDL:
            {
                cpu->execute.result_bus.buffer = cpu->execute.rs1_value + cpu->execute.imm ;
                cpu->execute.result_bus.tag = cpu->execute.rd;

                
                break;
            }

            case OPCODE_SUB:
            {
                cpu->execute.result_bus.buffer = (cpu->execute.rs1_value) - (cpu->execute.rs2_value);
                cpu->execute.result_bus.tag = cpu->execute.rd;

                
                break;
            }

            case OPCODE_SUBL:
            {
                cpu->execute.result_bus.buffer = cpu->execute.rs1_value - cpu->execute.imm ;
                cpu->execute.result_bus.tag = cpu->execute.rd;

                
                break;
            }

            case OPCODE_MUL:
            {
                cpu->execute.result_bus.buffer = cpu->execute.rs1_value *  (cpu->execute.rs2_value) ;
                cpu->execute.result_bus.tag = cpu->execute.rd;

                
                break;
            }
            case OPCODE_DIV:
            {
                cpu->execute.result_bus.buffer = (cpu->execute.rs1_value) * (1 / (cpu->execute.rs2_value) );
                cpu->execute.result_bus.tag = cpu->execute.rd;

                
                break;
            }


            case OPCODE_AND:
            {
                cpu->execute.result_bus.buffer = (cpu->execute.rs1_value) & (cpu->execute.rs2_value);
                cpu->execute.result_bus.tag = cpu->execute.rd;

                
                break;
            }

            case OPCODE_OR:
            {
                cpu->execute.result_bus.buffer = (cpu->execute.rs1_value )|(cpu->execute.rs2_value);
                cpu->execute.result_bus.tag = cpu->execute.rd;

                
                break;
            }

            case OPCODE_XOR:
            {
                cpu->execute.result_bus.buffer = (cpu->execute.rs1_value )^(cpu->execute.rs2_value);
                cpu->execute.result_bus.tag = cpu->execute.rd;

                
                break;
            }

            case OPCODE_LOAD:
            {
                cpu->execute.result_bus.buffer = cpu->execute.rs1_value + cpu->execute.imm;
                cpu->execute.result_bus.tag = cpu->execute.rd;

                
                break;
            }

            case OPCODE_LDR:
            {
                cpu->execute.result_bus.buffer = cpu->execute.rs1_value + cpu->execute.rs2_value;
                cpu->execute.result_bus.tag = cpu->execute.rd;
                
                
                break;
            }

            case OPCODE_STORE:
            {
                cpu->execute.result_bus.buffer = cpu->execute.rs1_value + cpu->execute.imm;
                break;
            }

            case OPCODE_STR:
            {
                cpu->execute.result_bus.buffer = cpu->execute.rs1_value + cpu->execute.rs2_value;
                break;
            }

            case OPCODE_CMP:
            {
                if (cpu->execute.rs1_value == cpu->execute.rs2_value)
                {
                    cpu->zero_flag = TRUE;
                    break;
                }
                else 
                {
                    cpu->zero_flag = FALSE;
                    break;
                }
            }


            case OPCODE_BZ:
            {
                if (cpu->zero_flag == TRUE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->execute.pc + cpu->execute.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                        * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_BNZ:
            {
                if (cpu->zero_flag == FALSE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->execute.pc + cpu->execute.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                        * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_MOVC: 
            {
                cpu->execute.result_bus.buffer = cpu->execute.imm;
                
                /* Set the zero flag based on the result buffer */
                if (cpu->execute.result_bus.buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }
            case OPCODE_HALT:
            {
                break;
            }
            case OPCODE_NOP:
            {
                printf("NOP");
                break;
            }
        }

        /* Copy data from execute latch to memory latch*/
        cpu->memory = cpu->execute;
        cpu->execute.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute", &cpu->execute);
        }
        
    }
    else
        {
            printf("Execute : Empty\n");
        }
}

/*
 * Memory Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_memory(APEX_CPU *cpu)
{
    if (cpu->memory.has_insn )
    {
        switch (cpu->memory.opcode)
        {
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_MUL:
            case OPCODE_DIV:
            case OPCODE_ADDL:
            case OPCODE_SUBL:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_XOR:
            case OPCODE_CMP:
            case OPCODE_MOVC:
            {
                /* No work for ADD */
                
                break;
            }

            case OPCODE_LOAD:
            case OPCODE_LDR:
            {
                /* Read from data memory */
                cpu->memory.result_bus.buffer = cpu->data_memory[cpu->memory.memory_address];
                cpu->memory.result_bus.tag = cpu->memory.rd;

                
                break;
            }

            case OPCODE_STORE:
            {
                cpu->memory.result_bus.buffer = cpu->memory.rs1_value;
                
                break;
            }
            case OPCODE_STR:
            {
                cpu->memory.result_bus.buffer = cpu->memory.rs1_value;
                
                break;
            }
            case OPCODE_HALT:
            {
                break;
            }
            case OPCODE_NOP:
            {
                printf("NOP");
                break;
            }
            
        }

        /* Copy data from memory latch to writeback latch*/
        cpu->writeback = cpu->memory;
        cpu->memory.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Memory", &cpu->memory);
        }
        
    }
    else
        {
            printf("Memory : Empty\n");
        }
}

/*
 * Writeback Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static int
APEX_writeback(APEX_CPU *cpu)
{
    if (cpu->writeback.has_insn)
    {
        /* Write result to register file based on instruction type */
        switch (cpu->writeback.opcode)
        {
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_MUL:
            case OPCODE_DIV:
            case OPCODE_ADDL:
            case OPCODE_SUBL:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_XOR:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_bus.buffer;
                
                break;
            }

            case OPCODE_LOAD:
            case OPCODE_LDR:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_bus.buffer;
                
                break;
            }

            case OPCODE_MOVC: 
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_bus.buffer;
                
                break;
            }

            case OPCODE_STORE:
            case OPCODE_STR:
            case OPCODE_CMP:
            {
                break;
            }
            case OPCODE_NOP:
            {
                printf("NOP");
                break;
            }
        }

        cpu->insn_completed++;
        cpu->writeback.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Writeback", &cpu->writeback);
        }
        

        if (cpu->writeback.opcode == OPCODE_HALT)
        {
            /* Stop the APEX simulator */
            //printf("WRItebAck ");
            return TRUE;
        }
        
    }
    else
        {
            printf("Writeback : Empty\n");
        }
    /* Default */
    return 0;
}

/*
 * This function creates and initializes APEX cpu.
 *
 * Note: You are free to edit this function according to your implementation
 */
APEX_CPU *
APEX_cpu_init(const char *filename,const char *disp_sim)
{
    int i;
    APEX_CPU *cpu;

    if (!filename)
    {
        return NULL;
    }
    
    cpu = calloc(1, sizeof(APEX_CPU));

    if (!cpu)
    {
        return NULL;
    }

    /* Initialize PC, Registers and all pipeline stages */
    cpu->pc = 4000;
    memset(cpu->regs, 0, sizeof(int) * REG_FILE_SIZE); 
    
    //Making it valid
    memset(cpu->regs_status, 0 ,sizeof(int) * REG_FILE_SIZE);
    
    for (int i=0;i<16;i++)
    {
        cpu->regs_status[i]= 0;
    }
    memset(cpu->data_memory, 0, sizeof(int) * DATA_MEMORY_SIZE);

    if(strcmp(disp_sim,"display") == 0)
    {
        cpu->single_step = ENABLE_SINGLE_STEP;
    }
    else if (strcmp(disp_sim,"simulate") == 0)
    {
        cpu->single_step = DISABLE_SINGLE_STEP;
    }
    

    /* Parse input file and create code memory */
    cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);
    if (!cpu->code_memory)
    {
        free(cpu);
        return NULL;
    }

    cpu->fetch.checker = 0;
    cpu->decode.checker = 0;
    cpu->execute.checker = 0;
    cpu->memory.checker = 0;
    cpu->writeback.checker = 0;
    

    if (ENABLE_DEBUG_MESSAGES)
    {
        fprintf(stderr,
                "APEX_CPU: Initialized APEX CPU, loaded %d instructions\n",
                cpu->code_memory_size);
        fprintf(stderr, "APEX_CPU: PC initialized to %d\n", cpu->pc);
        fprintf(stderr, "APEX_CPU: Printing Code Memory\n");
        printf("%-9s %-9s %-9s %-9s %-9s\n", "opcode_str", "rd", "rs1", "rs2",
                "imm");

        for (i = 0; i < cpu->code_memory_size; ++i)
        {
            printf("%-9s %-9d %-9d %-9d %-9d\n", cpu->code_memory[i].opcode_str,
                    cpu->code_memory[i].rd, cpu->code_memory[i].rs1,
                    cpu->code_memory[i].rs2, cpu->code_memory[i].imm);
        }
        
    }

    /* To start fetch stage */
    cpu->fetch.has_insn = TRUE;
    return cpu;
}

/*
 * APEX CPU simulation loop
 *
 * Note: You are free to edit this function according to your implementation
 */
void
APEX_cpu_run(APEX_CPU *cpu,const int cycles_expected)
{
    char user_prompt_val;

    while (TRUE)
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("--------------------------------------------\n");
            printf("Clock Cycle #: %d\n", cpu->clock);
            printf("--------------------------------------------\n");
        }

        if (APEX_writeback(cpu))
        {
            /* Halt in writeback stage */
            printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", (cpu->clock), cpu->insn_completed);
            break;
        }

        APEX_memory(cpu);
        APEX_execute(cpu);
        APEX_decode(cpu);
        APEX_fetch(cpu);

        print_reg_file(cpu);

        if (cpu->single_step)
        {
            printf("Press any key to advance CPU Clock or <q> to quit:\n");
            scanf("%c", &user_prompt_val);

            if ((user_prompt_val == 'Q') || (user_prompt_val == 'q'))
            {
                printf("APEX_CPU: Simulation Stopped, cycles = %d instructions = %d\n", (cpu->clock), cpu->insn_completed);
                break;
            }
        }
        if(cpu->clock == cycles_expected)
        {
            break;
        }
        else
        {
            cpu->clock++;
        }
        
    }
        printf("\n =============== STATE OF ARCHITECTURAL REGISTER FILE ========== \n");

        for (int i = 0; i<16;i++)
        {
            printf("Reg[%d] | Value = %d | Status =  VALID \n", i,cpu->regs[i]);
        }

        printf("\n ============== STATE OF DATA MEMORY ============= \n");

        for (int j=0; j<100;j++)
            {
                printf("MEM[%d] | Data Value = %d \n",j,cpu->data_memory[j]);
            }

}

/*
 * This function deallocates APEX CPU.
 *
 * Note: You are free to edit this function according to your implementation
 */
void
APEX_cpu_stop(APEX_CPU *cpu)
{    
    free(cpu->code_memory);
    free(cpu);
}