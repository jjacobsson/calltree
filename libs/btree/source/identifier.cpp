#include <btree/identifier.h>

void InitIdentifier( Identifier* id )
{
	id->m_Text = 0x0;
	id->m_Hash = INVALID_ID;
	id->m_Line = 0;
}
