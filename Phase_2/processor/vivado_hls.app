<project xmlns="com.autoesl.autopilot.project" name="processor" top="processor">
    <includePaths/>
    <libraryPaths/>
    <Simulation argv="">
        <SimFlow name="csim" setup="true" ldflags="" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../tb/processor_tb.cpp" sc="0" tb="1" cflags=" -I../include -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="processor/src/stages/write_back.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/stages/stage.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/components/register_map.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/processor.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/pipeline.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/units/memory_unit.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/stages/issue.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/architecture/immediate_packing.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/stages/fetch.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/architecture/disasm.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/stages/decode.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/debug/debugger.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/debug/debug_helpers.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/stages/commit.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/architecture/architecture_helpers.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
        <file name="processor/src/units/alu.cpp" sc="0" tb="false" cflags="-Iprocessor/include" csimflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

