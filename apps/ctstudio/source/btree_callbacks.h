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

#ifndef BTREE_CALLBACKS_H_INCLUDED
#define BTREE_CALLBACKS_H_INCLUDED

#include <QtCore/QFile>
#include <btree/btree_data.h>

struct ParsingInfo
{
  QFile* m_File;
  const char* m_Name;
};

int read_file( ParserContext pc, char* buffer, int maxsize );
void parser_error( ParserContext pc, const char* msg );
void parser_warning( ParserContext pc, const char* msg );
const char* parser_translate_include( ParserContext pc, const char* include );
void* allocate_memory( mem_size_t size );
void free_memory( void* ptr );


#endif //BTREE_CALLBACKS_H_INCLUDED
