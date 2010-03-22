/*******************************************************************************
 * Copyright (c) 2009-04-24 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <other/getopt.h>
#include <callback/callback.h>

#include "timing.h"

using namespace cb;


struct UserData
{
    uint64      m_TotalCounter;
    uint64      m_FrameCounter;
    double      m_FrameTime;
    int         m_GlobalCounter;
    char        m_String[4096];
    bool        m_Silent;
    bool        m_Exit;
};

void cb_debug( CallbackProgram* cp, DebugData* dd, Context* ctx )
{
  printf( "%s - %s %s\n", dd->m_Name, dd->m_Action, (dd->m_Flags&E_ENTER_SCOPE)?"Entry":"Exit" );
}

unsigned int cb_setexit(unsigned int action, void* bss, void** data, UserData& user_data)
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    user_data.m_Exit = true;
    return E_NODE_SUCCESS;
}

unsigned int cb_checkexit(unsigned int action, void* bss, void** data, UserData& user_data)
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    return user_data.m_Exit?E_NODE_SUCCESS:E_NODE_WORKING;
}

unsigned int cb_getline(unsigned int action, void* bss, void** data, UserData& ud)
{
    if (action != ACT_EXECUTE)
        return E_NODE_UNDEFINED;

    if( scanf( "%4095s", ud.m_String ) != 1 )
        return E_NODE_FAIL;

    return E_NODE_SUCCESS;
}

unsigned int cb_strcmp(unsigned int action, void* bss, void** data, UserData& user_data)
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    const char* const str_ptr = (const char* const)(data[0]);
    if( strcmp( user_data.m_String, str_ptr ) == 0 )
        return E_NODE_SUCCESS;
    return E_NODE_FAIL;
}

unsigned int cb_print(unsigned int action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    if( !ud.m_Silent )
    {
        const char* const str_ptr = (const char* const)(data[0]);
        printf("%s\n", str_ptr);
    }
    return E_NODE_SUCCESS;
}

unsigned int cb_count_to_zero(unsigned int action, void* bss, void** data, UserData& ud )
{
    if( action == ACT_CONSTRUCT )
    {
        *(int*)bss = *(int*)(data[0]);
        return E_NODE_UNDEFINED;
    }
    else if( action == ACT_EXECUTE )
    {
        return (((*(int*)bss)--) <= 0) ? E_NODE_SUCCESS : E_NODE_WORKING;
    }

    return E_NODE_UNDEFINED;
}

unsigned int cb_set_gc(unsigned int action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    ud.m_GlobalCounter = *(int*)(data[0]);
    return E_NODE_SUCCESS;
}

unsigned int cb_dec_gc(unsigned int action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    --ud.m_GlobalCounter;
    return E_NODE_SUCCESS;
}

unsigned int cb_inc_gc(unsigned int action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    ++ud.m_GlobalCounter;
    return E_NODE_SUCCESS;
}

unsigned int cb_check_gc_smlr(unsigned int action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    if( ud.m_GlobalCounter < *(int*)(data[0]) )
        return E_NODE_SUCCESS;
    return E_NODE_FAIL;
}

unsigned int cb_check_gc_grtr(unsigned int action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    if( ud.m_GlobalCounter > *(int*)(data[0]) )
        return E_NODE_SUCCESS;
    return E_NODE_FAIL;

}

unsigned int cb_time_delay( unsigned int action, void* bss, void** data, UserData& ud )
{
    double* t = (double*)bss;

    if( action == ACT_CONSTRUCT )
    {
        *t = (double)(*(float*)(data[0]));
        return E_NODE_UNDEFINED;
    }
    else if( action == ACT_EXECUTE )
    {
    	*t -= ud.m_FrameTime;
        if( *t <= 0.0f )
            return E_NODE_SUCCESS;
        return E_NODE_WORKING;
    }

    return E_NODE_UNDEFINED;
}

unsigned int cb_modify_return( unsigned int action, void* bss, void** data, UserData& ud )
{
    struct Parameters
    {
        int* m_Fail;
        int* m_Success;
        int* m_Running;
    };
    Parameters* params = (Parameters*)(data);
    int child_return   = *(int*)(bss);
    switch( child_return )
    {
    case E_NODE_FAIL:
        return *(params->m_Fail);
        break;
    case E_NODE_SUCCESS:
        return *(params->m_Success);
        break;
    case E_NODE_WORKING:
        return *(params->m_Running);
        break;
    default:
        break;
    }
    return E_NODE_UNDEFINED;
}

unsigned int cb_handler( CallData* cd, void* user_data )
{
    uint64 start, end;

    start = get_cpu_counter();

    UserData& ud = *((UserData*)user_data);

    unsigned int retVal = E_NODE_UNDEFINED;
    switch (cd->m_Id) {
    case 0:
        retVal = cb_setexit( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 1:
        retVal = cb_checkexit( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 2:
        retVal = cb_getline( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 3:
        retVal = cb_strcmp( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 4:
        retVal = cb_print( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 7:
        retVal = cb_count_to_zero( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 8:
        retVal = cb_set_gc( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 9:
        retVal = cb_dec_gc( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 10:
        retVal = cb_inc_gc( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 11:
        retVal = cb_check_gc_smlr( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 12:
        retVal = cb_check_gc_grtr( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 13:
        retVal = cb_time_delay( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    case 100:
        retVal = cb_modify_return( cd->m_Action, cd->m_Memory, cd->m_Parameters, ud );
        break;
    }

    end = get_cpu_counter();

    ((UserData*)user_data)->m_FrameCounter += end - start;
    ((UserData*)user_data)->m_TotalCounter += end - start;

    return retVal;
}

int main(int argc, char** argv)
{
    int returnCode = 0;
    char c = 0;
    char *inputFileName = 0x0;
    bool silent = false;

    GetOptContext ctx;
    init_getopt_context( &ctx );

    while ( (c = getopt(argc, argv, "?i:s", &ctx)) != -1)
    {
        switch (c)
        {
        case 'i':
            inputFileName = ctx.optarg;
            break;
        case 's':
            silent = true;
            break;
        case '?':
            printf("calltree testing application version 0.1\n\n");
            printf("Options:\n");
            printf("\t-i\tInput file\n");
            printf("\t-s\tSilent mode. Prevents the \"print\" action from echoing to the screen.\n" );
            printf("\t-?\tPrint this message and exit.\n\n");
            return 0;
            break;
        }
    }

    if ( !inputFileName)
    {
        printf("Error: You must specify an input file\n");
        returnCode = -1;
    }

    char*  program   = 0x0;
    size_t fileSize  = 0;
    FILE*  inputFile = 0x0;

    if (returnCode == 0)
        inputFile = fopen(inputFileName, "rb");

    if ( !inputFile)
    {
        printf("Error: Unable to open input file %s\n", inputFileName);
        returnCode = -2;
    }
    else
    {
        fseek(inputFile, 0, SEEK_END);
        fileSize = ftell(inputFile);
        fseek(inputFile, 0, SEEK_SET);

        program = (char*)malloc(fileSize);
        if (fileSize != fread(program, 1, fileSize, inputFile) )
        {
            printf("Error: could not read file %s\n", inputFileName);
            returnCode = -3;
        }

        fclose(inputFile);
    }

    if (returnCode == 0)
    {

        UserData ud;
        ud.m_Exit         = false;
        ud.m_TotalCounter = 0;
        ud.m_Silent       = silent;
        ud.m_FrameTime    = 0.0f;

        uint64 start, frame_start, end, frame_end, freq;

        ProgramHeader* ph = (ProgramHeader*)program;
        int memory_size = sizeof(Context) + ph->m_Bss + ph->m_Stack + ph->m_CStack;
        char* memory = (char*)alloca( memory_size );
        memset( memory, 0, memory_size );

        CallbackProgram cp;
        cp.m_Program  = program;
        cp.m_Memory   = memory;
        cp.m_UserData = (void*)&ud;
        cp.m_Callback = &cb_handler;
        cp.m_Debug    = &cb_debug;
        Context* ctx  = (Context*)memory;

        freq = get_cpu_frequency();

        unsigned int frames      = 0;
        unsigned int worst_frame = ~0;
        unsigned int best_frame  = ~0;
        unsigned int inst_worst  = 0;
        unsigned int inst_best   = 0;

        uint64 maxima = 0;
        uint64 minima = freq;

        double acc_ft = 0;
        start = get_cpu_counter();
        while( !ud.m_Exit )
        {
            ud.m_FrameCounter = 0;
            unsigned int pc = ctx->r[eic];

            frame_start = get_cpu_counter();

            run_program(&cp);
            ++frames;

            frame_end = get_cpu_counter();

            ud.m_FrameTime = (float)(((double)(frame_end - frame_start)) / (double)freq);
            acc_ft += ud.m_FrameTime;
            uint64 in_vm_this_frame = (frame_end - frame_start) - (ud.m_FrameCounter);

            if( in_vm_this_frame > maxima )
            {
                worst_frame = frames;
                maxima      = in_vm_this_frame;
                inst_worst  = ctx->r[eic] - pc;
            }
            if( in_vm_this_frame < minima )
            {
                best_frame = frames;
                minima     = in_vm_this_frame;
                inst_best  = ctx->r[eic] - pc;
            }
        }
        end = get_cpu_counter();

        uint64 total_ticks = end - start;
        uint64 time_in_vm  = total_ticks - ud.m_TotalCounter;
        double in_vm       = ((double)time_in_vm) / ((double)freq);
        double total       = ((double)total_ticks) / ((double)freq);

        printf( "Instructions executed:    %10d\n", ctx->r[eic] );
        printf( "Frames:                   %10d\n\n", frames );

        printf( "ys spent in VM per frame: %10.2f\n", (in_vm * 1000000.0) / (double)frames );
        printf( "ys in VM worst:           %10.2f\tFrame: %8d\tInst: %4d\n", (((double)maxima)* 1000000.0) / ((double)freq), worst_frame, inst_worst );
        printf( "ys in VM best:            %10.2f\tFrame: %8d\tInst: %4d\n\n", (((double)minima)* 1000000.0) / ((double)freq), best_frame, inst_best );

        printf( "s spent in VM:            %10.2f\n", in_vm );
        printf( "s spent in callbacks:     %10.2f\n", total - in_vm );
        printf( "Total Time in seconds:    %10.2f\n", total );
        printf( "Acc Time in seconds:      %10.2f\n", acc_ft );
        printf( "\n********************************************\n\n" );

    }

    if( program != 0x0 )
        free(program);

    return returnCode;
}
