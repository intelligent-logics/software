from fpga import Project



projdir=r'.\\autoquartus'

from fpga import Project

# Open an existing Quartus Prime project
project = Project('quartus', projdir)

# # Add HDL source files to the project
project.add_files('path/to/your/verilog/files/*.v')

# # Optionally add constraint files to the project
project.add_files('path/to/your/constraint/files/*.sdc')

# # Set the top-level unit name
# project.set_top('your_top_module')

# Generate the bitstream
project.generate()

project.program('de10_nano')




