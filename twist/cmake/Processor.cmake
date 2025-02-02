include(ProcessorCount)

# TODO: hyper-threading
ProcessorCount(ProcCount)
ProjectLog("Processor count: ${ProcCount}")

add_definitions(-DPROC_COUNT=${ProcCount})
