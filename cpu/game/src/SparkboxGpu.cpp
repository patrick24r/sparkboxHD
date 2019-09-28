static uint16_t SparkboxGpu::generateGpuCommand(commandType type, commandTarget target, uint16_t parameters)
{
    // returnVal[15:14] = type[1:0]
    // returnVal[13:11] = target[2:0]
    // returnVal[10:0] = parameters[10:0]
    return ((type & 0x03) << 14) | ((target & 0x07) << 11) | (parameters & 0x03FF);
}