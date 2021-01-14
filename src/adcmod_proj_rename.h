//------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2020 Zachary Cobell
//
// ADCIRCModules is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ADCIRCModules is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ADCIRCModules.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------//

// This file contains the defines for different proj configurations

#ifdef USE_INTERNAL_PROJ
#define adcmod_proj_context_get_database_path \
  internal_proj_context_get_database_path
#define adcmod_proj_create_crs_to_crs internal_proj_create_crs_to_crs
#define adcmod_proj_normalize_for_visualization \
  internal_proj_normalize_for_visualization
#define adcmod_proj_destroy internal_proj_destroy
#define adcmod_proj_angular_input internal_proj_angular_input
#define adcmod_proj_torad internal_proj_torad
#define adcmod_proj_trans internal_proj_trans
#define adcmod_proj_angular_output internal_proj_angular_output
#define adcmod_proj_todeg internal_proj_todeg
#define adcmod_proj_context_set_database_path \
  internal_proj_context_set_database_path
#else
#define adcmod_proj_context_get_database_path proj_context_get_database_path
#define adcmod_proj_create_crs_to_crs proj_create_crs_to_crs
#define adcmod_proj_normalize_for_visualization proj_normalize_for_visualization
#define adcmod_proj_destroy proj_destroy
#define adcmod_proj_angular_input proj_angular_input
#define adcmod_proj_torad proj_torad
#define adcmod_proj_trans proj_trans
#define adcmod_proj_angular_output proj_angular_output
#define adcmod_proj_todeg proj_todeg
#define adcmod_proj_context_set_database_path proj_context_set_database_path
#endif
