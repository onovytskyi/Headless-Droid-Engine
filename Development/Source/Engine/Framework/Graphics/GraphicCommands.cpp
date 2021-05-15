#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/GraphicCommands.h"

#include "Engine/Framework/Utils/CommandBuffer.h"
#include "Engine/Framework/Utils/CommandBufferReader.h"

namespace hd
{
    namespace gfx
    {
        template<typename Command>
        Command& WriteGraphicCommand(util::CommandBuffer& commandBuffer, GraphicCommandType commandType)
        {
            GraphicCommandType& commandID = commandBuffer.Write<GraphicCommandType>();
            commandID = commandType;

            return commandBuffer.Write<Command>();
        }

        ClearRenderTargetCommandCommand& ClearRenderTargetCommandCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<ClearRenderTargetCommandCommand>(commandBuffer, GraphicCommandType::ClearRenderTarget);
        }

        ClearRenderTargetCommandCommand& ClearRenderTargetCommandCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<ClearRenderTargetCommandCommand>();
        }
    }
}