C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe vert.vert -o vert.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe frag.frag -o frag.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe FrameBuffer.vert -o FrameBufferVert.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe FrameBuffer.frag -o FrameBufferFrag.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe SkyBoxFrameBufferShader.vert -o SkyBoxFrameBufferShaderVert.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe SkyBoxFrameBufferShader.frag -o SkyBoxFrameBufferShaderFrag.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe --target-env=vulkan1.1 --target-spv=spv1.4 closesthit.rchit -o closesthit.rchit.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe --target-env=vulkan1.1 --target-spv=spv1.4 anyhit.rahit -o anyhit.rahit.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe --target-env=vulkan1.1 --target-spv=spv1.4 anyhit1.rahit -o anyhit1.rahit.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe --target-env=vulkan1.1 --target-spv=spv1.4 raygen.rgen -o raygen.rgen.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe --target-env=vulkan1.1 --target-spv=spv1.4 miss.rmiss -o miss.rmiss.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe --target-env=vulkan1.1 --target-spv=spv1.4 shadow.rmiss -o shadow.rmiss.spv
C:/VulkanSDK/1.2.176.1/Bin32/glslc.exe animate.comp -o animate.spv
pause