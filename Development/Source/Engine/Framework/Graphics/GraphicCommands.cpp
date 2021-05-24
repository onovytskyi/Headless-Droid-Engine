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

        ClearRenderTargetCommand& ClearRenderTargetCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<ClearRenderTargetCommand>(commandBuffer, GraphicCommandType::ClearRenderTarget);
        }

        ClearRenderTargetCommand& ClearRenderTargetCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<ClearRenderTargetCommand>();
        }

        UpdateTextureCommand& UpdateTextureCommand::WriteTo(util::CommandBuffer& commandBuffer, size_t dataSize)
        {
            UpdateTextureCommand& command = WriteGraphicCommand<UpdateTextureCommand>(commandBuffer, GraphicCommandType::UpdateTexture);
            command.Data = &commandBuffer.Write<std::byte>(dataSize);
            command.Size = dataSize;

            return command;
        }

        UpdateTextureCommand& UpdateTextureCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            UpdateTextureCommand& command = commandBuffer.Read<UpdateTextureCommand>();
            commandBuffer.Read<std::byte>(command.Size);

            return command;
        }
    }
}