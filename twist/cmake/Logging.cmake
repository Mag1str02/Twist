function(ProjectLog message)
    message(STATUS "[${PROJECT_NAME}] ${message}")
endfunction()

function(ProjectWarning message)
    message(WARNING "[${PROJECT_NAME}] ${message}")
endfunction()
