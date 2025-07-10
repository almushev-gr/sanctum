#include "CommandNomenclature.h"
#include "SanctumDirCommand.h"
#include "WorkDirCommand.h"
#include "PutCommand.h"
#include <algorithm>


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
CommandNomenclature::CommandNomenclature(core::IfSanctumCore & core)
  : m_core(&core)
{
  m_cmds.emplace_back(std::make_unique<SanctumDirCommand>(*m_core));
  m_cmds.emplace_back(std::make_unique<WorkDirCommand>(*m_core));
  m_cmds.emplace_back(std::make_unique<PutCommand>(*m_core));
}


//----------------------------------------------------------
/*
  Получить команду по имени
*/
//---
IfCommand * CommandNomenclature::Get(const std::wstring & cmdName) const
{
  auto it = std::find_if(m_cmds.begin(), m_cmds.end(), 
  [&cmdName](const CommandPtr & nextCmd){ return nextCmd->GetName() == cmdName;} );

  if (it != m_cmds.end())
  {
    return &**it;
  }

  return nullptr;
}

}