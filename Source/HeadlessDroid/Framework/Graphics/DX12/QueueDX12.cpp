#include "Config/Bootstrap.h"

#include "Framework/Graphics/Queue.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Debug/Assert.h"
#include "Framework/Graphics/Device.h"
#include "Framework/Graphics/Fence.h"

namespace hd
{
	D3D12_COMMAND_LIST_TYPE ToNative(QueueType type)
	{
		switch (type)
		{
		case QueueType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case QueueType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case QueueType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
		default:
			hdAssert(false, u8"Unsupported command queue type.");
			break;
		}

		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	}

	QueuePlatform::QueuePlatform(Device& device, QueueType type)
		: m_OwnerDevice{ &device }
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.NodeMask = 0;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = ToNative(type);
		hdEnsure(device.GetNativeDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_Queue.GetAddressOf())));
	}

	QueuePlatform::~QueuePlatform()
	{

	}

	ID3D12CommandQueue* QueuePlatform::GetNativeQueue() const
	{
		return m_Queue.Get();
	}

	void QueuePlatform::PresentFrom(TextureHandle framebuffer)
	{
		m_OwnerDevice->PresentOnQueue(*static_cast<Queue*>(this), framebuffer);
	}

	void Queue::Signal(Fence& fence, uint64_t value)
	{
		m_Queue->Signal(fence.GetNativeFence(), value);
	}

	void Queue::Submit(CommandBuffer& commandBuffer)
	{
		m_OwnerDevice->SubmitToQueue(*static_cast<Queue*>(this), commandBuffer);
	}
}

#endif
