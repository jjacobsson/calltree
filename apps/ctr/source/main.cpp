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

#include <callback/types.h>
#include <other/getopt.h>
#include <callback/callback.h>
#include <callback/instructions.h>

#include "timing.h"

using namespace callback;

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

void cb_debug( const char* info, uint32 RE, uint32 IP )
{
    const char* const str_RE[MAXIMUM_NODE_RETURN_COUNT] = {
        "E_NODE_FAIL",
        "E_NODE_SUCCESS",
        "E_NODE_RUNNING",
        "E_NODE_UNDEFINED"
    };

    printf( "%s 0x%04x %s\n", info, IP, str_RE[RE] );
}

uint32 cb_setexit(uint32 action, void* bss, void** data, UserData& user_data)
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    user_data.m_Exit = true;
    return E_NODE_SUCCESS;
}

uint32 cb_checkexit(uint32 action, void* bss, void** data, UserData& user_data)
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    return user_data.m_Exit?E_NODE_SUCCESS:E_NODE_RUNNING;
}

uint32 cb_getline(uint32 action, void* bss, void** data, UserData& ud)
{
    if (action != ACT_EXECUTE)
        return E_NODE_UNDEFINED;

    if( scanf( "%4095s", ud.m_String ) != 1 )
        return E_NODE_FAIL;

    return E_NODE_SUCCESS;
}

uint32 cb_strcmp(uint32 action, void* bss, void** data, UserData& user_data)
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    const char* const str_ptr = (const char* const)(data[0]);
    if( strcmp( user_data.m_String, str_ptr ) == 0 )
        return E_NODE_SUCCESS;
    return E_NODE_FAIL;
}

uint32 cb_print(uint32 action, void* bss, void** data, UserData& ud )
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

uint32 cb_count_to_zero(uint32 action, void* bss, void** data, UserData& ud )
{
    if( action == ACT_CONSTRUCT )
    {
        *(int*)bss = *(int*)(data[0]);
        return E_NODE_UNDEFINED;
    }
    else if( action == ACT_EXECUTE )
    {
        return (((*(int*)bss)--) <= 0) ? E_NODE_SUCCESS : E_NODE_RUNNING;
    }

    return E_NODE_UNDEFINED;
}

uint32 cb_set_gc(uint32 action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    ud.m_GlobalCounter = *(int*)(data[0]);
    return E_NODE_SUCCESS;
}

uint32 cb_dec_gc(uint32 action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    --ud.m_GlobalCounter;
    return E_NODE_SUCCESS;
}

uint32 cb_inc_gc(uint32 action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    ++ud.m_GlobalCounter;
    return E_NODE_SUCCESS;
}

uint32 cb_check_gc_smlr(uint32 action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    if( ud.m_GlobalCounter < *(int*)(data[0]) )
        return E_NODE_SUCCESS;
    return E_NODE_FAIL;
}

uint32 cb_check_gc_grtr(uint32 action, void* bss, void** data, UserData& ud )
{
    if( action != ACT_EXECUTE )
        return E_NODE_UNDEFINED;

    if( ud.m_GlobalCounter > *(int*)(data[0]) )
        return E_NODE_SUCCESS;
    return E_NODE_FAIL;

}

uint32 cb_time_delay( uint32 action, void* bss, void** data, UserData& ud )
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
        return E_NODE_RUNNING;
    }

    return E_NODE_UNDEFINED;
}

uint32 cb_modify_return( uint32 action, void* bss, void** data, UserData& ud )
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
    case E_NODE_RUNNING:
        return *(params->m_Running);
        break;
    default:
        break;
    }
    return E_NODE_UNDEFINED;
}

uint32 cb_handler(uint32 id, uint32 action, void* bss, void** data, void* user_data)
{
    uint64 start, end;

    start = get_cpu_counter();

    UserData& ud = *((UserData*)user_data);

    uint32 retVal = E_NODE_UNDEFINED;
    switch (id) {
    case 0:
        retVal = cb_setexit( action, bss, data, ud );
        break;
    case 1:
        retVal = cb_checkexit( action, bss, data, ud );
        break;
    case 2:
        retVal = cb_getline( action, bss, data, ud );
        break;
    case 3:
        retVal = cb_strcmp( action, bss, data, ud );
        break;
    case 4:
        retVal = cb_print( action, bss, data, ud );
        break;
    case 5:
        retVal = E_NODE_FAIL;
        break;
    case 6:
        retVal = E_NODE_SUCCESS;
        break;
    case 7:
        retVal = cb_count_to_zero( action, bss, data, ud );
        break;
    case 8:
        retVal = cb_set_gc( action, bss, data, ud );
        break;
    case 9:
        retVal = cb_dec_gc( action, bss, data, ud );
        break;
    case 10:
        retVal = cb_inc_gc( action, bss, data, ud );
        break;
    case 11:
        retVal = cb_check_gc_smlr( action, bss, data, ud );
        break;
    case 12:
        retVal = cb_check_gc_grtr( action, bss, data, ud );
        break;
    case 13:
        retVal = cb_time_delay( action, bss, data, ud );
        break;
    case 100:
        retVal = cb_modify_return( action, bss, data, ud );
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
    int count = 0;
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

        int bss_size = ((ProgramHeader*)program)->m_BS;
        char* bss = (char*)alloca( bss_size );
        memset( bss, 0, bss_size );

        CallbackProgram cp;
        cp.m_Program  = program;
        cp.m_bss      = bss;
        cp.m_UserData = (void*)&ud;
        cp.m_callback = &cb_handler;
        //cp.m_Debug  = &cb_debug;
        BssHeader* bh = (BssHeader*)bss;

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
            unsigned int IC = bh->m_IC;

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
                inst_worst  = bh->m_IC - IC;
            }
            if( in_vm_this_frame < minima )
            {
                best_frame = frames;
                minima     = in_vm_this_frame;
                inst_best  = bh->m_IC - IC;
            }
        }
        end = get_cpu_counter();

        uint64 total_ticks = end - start;
        uint64 time_in_vm  = total_ticks - ud.m_TotalCounter;
        double in_vm       = ((double)time_in_vm) / ((double)freq);
        double total       = ((double)total_ticks) / ((double)freq);

        printf( "Instructions executed:    %10d\n", bh->m_IC );
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
