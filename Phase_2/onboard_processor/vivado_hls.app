<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="com.autoesl.autopilot.project" name="onboard_processor" top="processor">
  <files>
    <file name="../tb/processor_tb.cpp" sc="0" tb="1" cflags=" -I../include -Wno-unknown-pragmas" blackbox="false" csimflags=" -Wno-unknown-pragmas"/>
    <file name="onboard_processor/src/components/register_map.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
    <file name="onboard_processor/src/processor.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
    <file name="onboard_processor/src/pipeline.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
    <file name="onboard_processor/src/stages/issue.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
    <file name="onboard_processor/src/architecture/immediate_packing.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
    <file name="onboard_processor/src/stages/fetch.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
    <file name="onboard_processor/src/stages/decode.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
    <file name="onboard_processor/src/debug/debugger.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
    <file name="onboard_processor/src/debug/debug_helpers.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
    <file name="onboard_processor/src/stages/commit.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
    <file name="onboard_processor/src/architecture/architecture_helpers.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" blackbox="false" csimflags=""/>
  </files>
  <solutions>
    <solution name="solution" status="active"/>
  </solutions>
  <includePaths/>
  <libraryPaths/>
  <Simulation>
    <SimFlow name="csim" csimMode="0" lastCsimMode="0"/>
  </Simulation>
</project>
