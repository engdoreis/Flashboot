#*********************************************************************************************
# @file     
# @project  
# @platform 
# @device   
# @author   Douglas Reis
# @version  0.1
# @date     06-Feb-2019
# @brief    Configure the source to be use during compilation of the Project    
#
# @history
#
# When         Who               What
# -----------  ----------------  -------------------------------------------------------------
# 06-Feb-2019  Douglas Reis     - Initial Version. 
# ********************************************************************************************

# Make does not offer a recursive wildcard function, so here's one for files:
rfwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rfwildcard,$d/,$2))

# Make does not offer a recursive wildcard function, so here's one for directories:
rdwildcard=$(sort $(dir $(call rfwildcard,$1,$2)))
