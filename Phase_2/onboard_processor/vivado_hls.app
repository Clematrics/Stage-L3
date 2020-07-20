<project xmlns="com.autoesl.autopilot.project" name="onboard_processor" top="processor">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../tb/processor_tb.cpp" sc="0" tb="1" cflags=" -I../include -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="onboard_processor/src/processor.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" csimflags="" blackbox="false"/>
        <file name="onboard_processor/src/pipeline.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" csimflags="" blackbox="false"/>
        <file name="onboard_processor/src/stages/fetch.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" csimflags="" blackbox="false"/>
        <file name="onboard_processor/src/stages/decode.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" csimflags="" blackbox="false"/>
        <file name="onboard_processor/src/debug/debugger.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" csimflags="" blackbox="false"/>
        <file name="onboard_processor/src/debug/debug_helpers.cpp" sc="0" tb="false" cflags="-Ionboard_processor/include" csimflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution" status="active"/>
    </solutions>
</project>

