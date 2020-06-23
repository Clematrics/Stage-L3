<project xmlns="com.autoesl.autopilot.project" name="pipeline" top="pipeline">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" csimMode="0" lastCsimMode="2"/>
    </Simulation>
    <files xmlns="">
        <file name="../tb/pipeline_tb.cpp" sc="0" tb="1" cflags=" -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="pipeline/include/pipeline.hpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="pipeline/src/pipeline.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="pipeline/include/instructions.hpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="pipeline/include/fetch.hpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="pipeline/src/fetch.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="pipeline/include/decode.hpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="pipeline/src/decode.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="pipeline/include/common.hpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

