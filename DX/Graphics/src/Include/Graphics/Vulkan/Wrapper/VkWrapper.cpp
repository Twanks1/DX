#include "VkWrapper.h"
/**********************************************************************
    class: Several

    author: S. Hau
    date: August 23, 2018
**********************************************************************/

#include "../Vulkan.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // IVulkanResource
    //**********************************************************************

    //----------------------------------------------------------------------
    bool IVulkanResource::release()
    {
        m_useCount--;
        if (m_useCount == 0)
        {
            vkDeviceWaitIdle( g_vulkan.device );
            delete this;
        }
        return m_useCount == 0;
    }

    //----------------------------------------------------------------------
    void IVulkanResource::addRef()
    {
        m_useCount++;
    }

    //**********************************************************************
    // ColorImage
    //**********************************************************************

    //----------------------------------------------------------------------
    ColorImage::ColorImage( U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VmaMemoryUsage memUsage )
        : IImage( width, height, format, samples, VK_IMAGE_LAYOUT_UNDEFINED )
    {
        VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        createInfo.imageType     = VK_IMAGE_TYPE_2D;
        createInfo.format        = format;
        createInfo.extent        = { width, height, 1 };
        createInfo.mipLevels     = 1;
        createInfo.arrayLayers   = 1;
        createInfo.samples       = samples;
        createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage         = usage;
        createInfo.initialLayout = layout;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memUsage;
        vmaCreateImage( g_vulkan.allocator, &createInfo, &allocInfo, &img, &mem, nullptr );
    }

    //----------------------------------------------------------------------
    ColorImage::ColorImage( VkImage image, U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples )
        : IImage( width, height, format, samples, VK_IMAGE_LAYOUT_UNDEFINED )
    {
        this->img = image;
    }

    //----------------------------------------------------------------------
    ColorImage::~ColorImage()
    {
        if (img && mem)
        {
            vmaDestroyImage( g_vulkan.allocator, img, mem );
            img = VK_NULL_HANDLE;
        }
    }

    //**********************************************************************
    // DepthImage
    //**********************************************************************

    //----------------------------------------------------------------------
    DepthImage::DepthImage( U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples )
        : IImage( width, height, format, samples, VK_IMAGE_LAYOUT_UNDEFINED )
    {
        VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        createInfo.imageType     = VK_IMAGE_TYPE_2D;
        createInfo.format        = format;
        createInfo.extent        = { width, height, 1 };
        createInfo.mipLevels     = 1;
        createInfo.arrayLayers   = 1;
        createInfo.samples       = samples;
        createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        createInfo.initialLayout = layout;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        vmaCreateImage( g_vulkan.allocator, &createInfo, &allocInfo, &img, &mem, nullptr );
    }

    //----------------------------------------------------------------------
    DepthImage::~DepthImage()
    {
        if (img && mem)
        {
            vmaDestroyImage( g_vulkan.allocator, img, mem );
            img = VK_NULL_HANDLE;
        }
    }

    //**********************************************************************
    // ImageView
    //**********************************************************************

    //----------------------------------------------------------------------
    ImageView::ImageView( ColorImage* image )
        : img( image )
    {
        img->addRef();

        VkImageViewCreateInfo viewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewCreateInfo.image                        = image->img;
        viewCreateInfo.viewType                     = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format                       = image->format;
        viewCreateInfo.subresourceRange.aspectMask  = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.layerCount  = VK_REMAINING_ARRAY_LAYERS;
        viewCreateInfo.subresourceRange.levelCount  = VK_REMAINING_MIP_LEVELS;
        vkCreateImageView( g_vulkan.device, &viewCreateInfo, ALLOCATOR, &view );
    }

    //----------------------------------------------------------------------
    ImageView::ImageView( DepthImage* image )
        : img( image )
    {
        img->addRef();

        VkImageViewCreateInfo viewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewCreateInfo.image                        = image->img;
        viewCreateInfo.viewType                     = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format                       = image->format;
        viewCreateInfo.subresourceRange.aspectMask  = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewCreateInfo.subresourceRange.layerCount  = VK_REMAINING_ARRAY_LAYERS;
        viewCreateInfo.subresourceRange.levelCount  = VK_REMAINING_MIP_LEVELS;
        vkCreateImageView( g_vulkan.device, &viewCreateInfo, ALLOCATOR, &view );
    }

    //----------------------------------------------------------------------
    ImageView::~ImageView()
    {
        img->release();

        if (view)
        {
            vkDestroyImageView( g_vulkan.device, view, ALLOCATOR );
            view = VK_NULL_HANDLE;
        }
    }

    //**********************************************************************
    // RenderPass
    //**********************************************************************

    //----------------------------------------------------------------------
    RenderPass::RenderPass( const AttachmentDescription& color, const AttachmentDescription& depth )
    {
        ArrayList<VkAttachmentDescription> attachments;

        VkSubpassDescription subPass{};
        subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkAttachmentReference colorAttachment{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        if (color.img)
        {
            subPass.colorAttachmentCount = 1;
            subPass.pColorAttachments = &colorAttachment;
            VkAttachmentDescription attachment{};
            attachment.format           = color.img->format;
            attachment.samples          = color.img->samples;
            attachment.loadOp           = color.loadOp;
            attachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout    = color.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD ? color.img->layout : VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout      = color.finalLayout;
            attachments.push_back( attachment );
            newLayouts.push_back( color.finalLayout );
        }

        VkAttachmentReference depthAttachment{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
        if (depth.img)
        {
            subPass.pDepthStencilAttachment = &depthAttachment;

            VkAttachmentDescription attachment{};
            attachment.format           = depth.img->format;
            attachment.samples          = depth.img->samples;
            attachment.loadOp           = depth.loadOp;
            attachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout    = depth.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD ? depth.img->layout : VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout      = depth.finalLayout;
            attachments.push_back( attachment );
            newLayouts.push_back( depth.finalLayout );
        }

        std::array<VkSubpassDependency, 2> dependencies{};
        dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass      = 0;
        dependencies[0].srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass      = 0;
        dependencies[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo createInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        createInfo.attachmentCount  = (U32)attachments.size();
        createInfo.pAttachments     = attachments.data();
        createInfo.subpassCount     = 1;
        createInfo.pSubpasses       = &subPass;
        createInfo.dependencyCount  = (U32)dependencies.size();
        createInfo.pDependencies    = dependencies.data();

        vkCreateRenderPass( g_vulkan.device, &createInfo, ALLOCATOR, &renderPass );
    }

    //----------------------------------------------------------------------
    RenderPass::~RenderPass()
    {
        if (renderPass)
        {
            vkDestroyRenderPass( g_vulkan.device, renderPass, ALLOCATOR );
            renderPass = VK_NULL_HANDLE;
        }
    }

    //**********************************************************************
    // Framebuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    Framebuffer::Framebuffer( RenderPass* renderPass, ImageView* colorView, ImageView* depthView )
    {
        ArrayList<VkImageView> views;
        VkFramebufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        if (colorView)
        {
            attachments.push_back( colorView );
            colorView->addRef();
            views.push_back( colorView->view );
        }
        if (depthView)
        {
            attachments.push_back( depthView );
            depthView->addRef();
            views.push_back( depthView->view );
        }
        createInfo.renderPass       = renderPass->renderPass;
        createInfo.attachmentCount  = (U32)views.size();
        createInfo.pAttachments     = views.data();
        createInfo.width            = getWidth();
        createInfo.height           = getHeight();
        createInfo.layers           = 1;

        vkCreateFramebuffer( g_vulkan.device, &createInfo, ALLOCATOR, &framebuffer );
    }

    //----------------------------------------------------------------------
    Framebuffer::~Framebuffer()
    {
        for (auto& attachment : attachments)
            attachment->release();
        if (framebuffer)
        {
            vkDestroyFramebuffer( g_vulkan.device, framebuffer, ALLOCATOR );
            framebuffer = VK_NULL_HANDLE;
        }
    }

    //----------------------------------------------------------------------
    U32 Framebuffer::getWidth()
    {
        return attachments.front()->img->width;
    }

    //----------------------------------------------------------------------
    U32 Framebuffer::getHeight()
    {
        return attachments.front()->img->height;
    }

    //**********************************************************************
    // CommandBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    CmdBuffer::CmdBuffer( U32 queueFamilyIndex, VkCommandPoolCreateFlags poolFlags, VkFenceCreateFlags fenceFlags )
    {
        VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        createInfo.flags = poolFlags;
        createInfo.queueFamilyIndex = queueFamilyIndex;
        vkCreateCommandPool( g_vulkan.device, &createInfo, ALLOCATOR, &pool );

        VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocateInfo.commandPool = pool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;
        vkAllocateCommandBuffers( g_vulkan.device, &allocateInfo, &cmd );

        VkFenceCreateInfo fenceCreateInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fenceCreateInfo.flags = fenceFlags;
        vkCreateFence( g_vulkan.device, &fenceCreateInfo, ALLOCATOR, &fence );
    }

    //----------------------------------------------------------------------
    CmdBuffer::~CmdBuffer()
    {
        if (pool)
        {
            vkDestroyCommandPool( g_vulkan.device, pool, ALLOCATOR );
            pool = VK_NULL_HANDLE;
        }
        if (fence)
        {
            vkDestroyFence( g_vulkan.device, fence, ALLOCATOR );
            fence = VK_NULL_HANDLE;
        }
        cmd = VK_NULL_HANDLE;
    }

    //----------------------------------------------------------------------
    void CmdBuffer::begin( VkCommandBufferUsageFlags flags )
    {
        VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = flags;
        vkBeginCommandBuffer( cmd, &beginInfo );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::exec( VkQueue queue, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore )
    {
        vkEndCommandBuffer( cmd );

        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        if (waitSemaphore != VK_NULL_HANDLE)
        {
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = &waitSemaphore;
            submitInfo.pWaitDstStageMask    = &waitDstStageMask;
        }
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &cmd;
        if (signalSemaphore != VK_NULL_HANDLE)
        {
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &signalSemaphore;
        }

        vkQueueSubmit( queue, 1, &submitInfo, fence );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::wait()
    {
        const uint32_t timeoutNs = 1000 * 1000 * 1000;
        vkWaitForFences( g_vulkan.device, 1, &fence, VK_FALSE, timeoutNs );
        vkResetFences( g_vulkan.device, 1, &fence );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::reset()
    {
        VkCommandBufferResetFlags flags{};
        vkResetCommandBuffer( cmd, flags );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::setImageLayout( ColorImage* img, VkImageLayout newLayout,
                                                     VkAccessFlags srcAccess, VkAccessFlags dstAccess,
                                                     VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask )
    {
        VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.srcAccessMask       = srcAccess;
        barrier.dstAccessMask       = dstAccess;
        barrier.oldLayout           = img->layout;
        barrier.newLayout           = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; 
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image               = img->img;
        barrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0,  VK_REMAINING_ARRAY_LAYERS };

        vkCmdPipelineBarrier( cmd, srcStageMask, dstStageMask, 0,
                               0, VK_NULL_HANDLE,
                               0, VK_NULL_HANDLE,
                               1, &barrier);
        img->layout = newLayout;
    }

    //----------------------------------------------------------------------
    void CmdBuffer::resolveImage( ColorImage* src, ColorImage* dst )
    {
        VkImageResolve region{};
        region.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, VK_REMAINING_ARRAY_LAYERS };
        region.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, VK_REMAINING_ARRAY_LAYERS };
        region.extent = { src->width, dst->width };
        vkCmdResolveImage( cmd, src->img, src->layout, dst->img, dst->layout, 1, &region );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::resolveImage( DepthImage* src, DepthImage* dst )
    {
        VkImageResolve region{};
        region.srcSubresource = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 0, VK_REMAINING_ARRAY_LAYERS };
        region.dstSubresource = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 0, VK_REMAINING_ARRAY_LAYERS };
        region.extent = { src->width, dst->width };
        vkCmdResolveImage( cmd, src->img, src->layout, dst->img, dst->layout, 1, &region );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::beginRenderPass( RenderPass* renderPass, Framebuffer* fbo, std::array<VkClearValue, 2> clearValues )
    {
        VkRenderPassBeginInfo beginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        beginInfo.renderPass        = renderPass->renderPass;
        beginInfo.framebuffer       = fbo->framebuffer;
        beginInfo.renderArea.extent = { fbo->getWidth(), fbo->getHeight() };
        beginInfo.clearValueCount   = (U32)clearValues.size();
        beginInfo.pClearValues      = clearValues.data();

        vkCmdBeginRenderPass( cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE );

        // Update layout from images in the fbo
        for (I32 i = 0; i < fbo->attachments.size(); ++i)
            fbo->attachments[i]->img->layout = renderPass->newLayouts[i];
    }

    //----------------------------------------------------------------------
    void CmdBuffer::endRenderPass()
    {
        vkCmdEndRenderPass( cmd );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::bindGraphicsPipeline( VkPipeline pipeline )
    {
        vkCmdBindPipeline( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::setViewport( VkViewport viewport )
    {
        vkCmdSetViewport( cmd, 0, 1, &viewport );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::draw( U32 vertexCount, U32 instanceCount, U32 firstVertex, U32 firstInstance )
    {
        vkCmdDraw( cmd, vertexCount, instanceCount, firstVertex, firstInstance );
    }

    //**********************************************************************
    // Pipeline
    //**********************************************************************

    //----------------------------------------------------------------------
    void GraphicsPipeline::addShaderModule( VkShaderStageFlagBits shaderStage, VkShaderModule shaderModule, CString entryPoint )
    {
        VkPipelineShaderStageCreateInfo shaderStageInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        shaderStageInfo.stage  = shaderStage;
        shaderStageInfo.module = shaderModule;
        shaderStageInfo.pName  = entryPoint;

        m_shaderStages.push_back( shaderStageInfo );
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::setVertexInputState( const ArrayList<VkVertexInputBindingDescription>& inputDesc, const ArrayList<VkVertexInputAttributeDescription>& attrDesc )
    {
        m_vertexInputBindingDesc = inputDesc;
        m_vertexInputAttrDesc    = attrDesc;
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::setInputAssemblyState( VkPrimitiveTopology topology, VkBool32 restartEnable )
    {
        m_inputAssemblyState.primitiveRestartEnable = restartEnable;
        m_inputAssemblyState.topology               = topology;
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::setRasterizationState( VkBool32 depthClampEnable, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace )
    {
        m_rasterizationState.depthClampEnable = depthClampEnable;
        m_rasterizationState.polygonMode      = polygonMode;
        m_rasterizationState.cullMode         = cullMode;
        m_rasterizationState.frontFace        = frontFace;
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::setRasterizationState( VkBool32 depthBiasEnable, F32 depthBiasConstantFactor, F32 depthBiasClamp, F32 depthBiasSlopeFactor )
    {
        m_rasterizationState.depthBiasEnable         = depthBiasEnable;
        m_rasterizationState.depthBiasConstantFactor = depthBiasConstantFactor;
        m_rasterizationState.depthBiasClamp          = depthBiasClamp;
        m_rasterizationState.depthBiasSlopeFactor    = depthBiasSlopeFactor;
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::setMultisampleState( VkSampleCountFlagBits samples, VkBool32 alphaToCoverabeEnable, VkBool32 alphaToOneEnable )
    {
        m_multisampleState.rasterizationSamples  = samples;
        m_multisampleState.alphaToCoverageEnable = alphaToCoverabeEnable;
        m_multisampleState.alphaToOneEnable      = alphaToOneEnable;
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::setMultisampleState( VkBool32 sampleShadingEnable, F32 minSampleShading )
    {
        m_multisampleState.sampleShadingEnable = sampleShadingEnable;
        m_multisampleState.minSampleShading    = minSampleShading;
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::setDepthStencilState( VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp )
    {
        m_depthStencilState.depthTestEnable  = depthTestEnable;
        m_depthStencilState.depthWriteEnable = depthWriteEnable;
        m_depthStencilState.depthCompareOp   = depthCompareOp;
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::setDepthStencilState( VkBool32 stencilTestEnable, VkStencilOpState front, VkStencilOpState back )
    {
        m_depthStencilState.stencilTestEnable = stencilTestEnable;
        m_depthStencilState.front             = front;
        m_depthStencilState.back              = back;
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::setDepthStencilState( VkBool32 depthBoundsTestEnable, F32 minBounds, F32 maxBounds )
    {
        m_depthStencilState.depthBoundsTestEnable = depthBoundsTestEnable;
        m_depthStencilState.minDepthBounds        = minBounds;
        m_depthStencilState.maxDepthBounds        = maxBounds;
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::addBlendAttachment( VkBlendFactor srcColorBlend, VkBlendFactor dstColorBlend, VkBlendOp colorBlendOp )
    {
        addBlendAttachment( srcColorBlend, dstColorBlend, colorBlendOp, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD );
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::addBlendAttachment( VkBlendFactor srcColorBlend, VkBlendFactor dstColorBlend, VkBlendOp colorBlendOp,
                                               VkBlendFactor srcAlphaBlend, VkBlendFactor dstAlphaBlend, VkBlendOp alphaBlendOp )
    {
        VkPipelineColorBlendAttachmentState blendState{};
        blendState.srcColorBlendFactor = srcColorBlend;
        blendState.dstColorBlendFactor = dstColorBlend;
        blendState.colorBlendOp        = colorBlendOp;
        blendState.srcAlphaBlendFactor = srcAlphaBlend;
        blendState.dstAlphaBlendFactor = dstAlphaBlend;
        blendState.alphaBlendOp        = alphaBlendOp;
        blendState.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        m_colorBlendAttachmentStates.push_back( blendState );
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::addDynamicState( VkDynamicState dynamicState )
    {
        m_dynamicStates.push_back( dynamicState );
    }

    //----------------------------------------------------------------------
    void GraphicsPipeline::buildPipeline( VkPipelineLayout layout, VkRenderPass renderPass, U32 subPass )
    {
        VkPipelineVertexInputStateCreateInfo vertexInputState{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputState.vertexBindingDescriptionCount    = (U32)m_vertexInputBindingDesc.size();
        vertexInputState.pVertexBindingDescriptions       = m_vertexInputBindingDesc.data();
        vertexInputState.vertexAttributeDescriptionCount  = (U32)m_vertexInputAttrDesc.size();
        vertexInputState.pVertexAttributeDescriptions     = m_vertexInputAttrDesc.data();

        m_colorBlendState.attachmentCount = (U32)m_colorBlendAttachmentStates.size();
        m_colorBlendState.pAttachments    = m_colorBlendAttachmentStates.data();

        VkPipelineDynamicStateCreateInfo dynamicState{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.dynamicStateCount  = (U32)m_dynamicStates.size();
        dynamicState.pDynamicStates     = m_dynamicStates.data();

        VkGraphicsPipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        createInfo.stageCount           = (U32)m_shaderStages.size();
        createInfo.pStages              = m_shaderStages.data();
        createInfo.pVertexInputState    = &vertexInputState;
        createInfo.pInputAssemblyState  = &m_inputAssemblyState;
        createInfo.pTessellationState   = VK_NULL_HANDLE;
        createInfo.pViewportState       = &m_viewportState;
        createInfo.pRasterizationState  = &m_rasterizationState;
        createInfo.pMultisampleState    = &m_multisampleState;
        createInfo.pDepthStencilState   = &m_depthStencilState;
        createInfo.pColorBlendState     = &m_colorBlendState;
        createInfo.pDynamicState        = &dynamicState;
        createInfo.layout               = layout;
        createInfo.renderPass           = renderPass;
        createInfo.subpass              = subPass;
        createInfo.basePipelineHandle   = VK_NULL_HANDLE;
        createInfo.basePipelineIndex    = VK_NULL_HANDLE;

        vkCreateGraphicsPipelines( g_vulkan.device, VK_NULL_HANDLE, 1, &createInfo, ALLOCATOR, &pipeline );
    }

    //----------------------------------------------------------------------
    GraphicsPipeline::~GraphicsPipeline()
    {
        if (pipeline)
        {
            vkDeviceWaitIdle( g_vulkan.device );
            vkDestroyPipeline( g_vulkan.device, pipeline, ALLOCATOR );
            pipeline = VK_NULL_HANDLE;
        }
    }

} } // End namespaces