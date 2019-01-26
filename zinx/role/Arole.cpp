#include "Arole.h"

Arole::Arole()
{

}

Arole::~Arole()
{
}

Achannel *Arole::GetChannel()
{
    return m_pxOutChannel;
}

void Arole::SetChannel(Achannel * pxChannel)
{
    m_pxOutChannel = pxChannel;
}