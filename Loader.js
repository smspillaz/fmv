function audio() {
    var context = new (window.webkitAudioContext || window.AudioContext)();
    var source;
    var processor;
    var analyser;
    var xhr;
    var disconnectTimeout = 0;

    function initAudio(data) {
        source = context.createBufferSource();
        
        if (context.decodeAudioData) {
            context.decodeAudioData(data, function(buffer) {
                source.buffer = buffer;
                createAudio();
            }, function(e) {
                console.log(e);
            });
        } else {
            source.buffer = context.createBuffer(data, false /*mixToMono*/);
            createAudio();
        }
    }

    function createAudio() {
        processor = context.createJavaScriptNode(512 /*bufferSize*/, 1 /*num inputs*/, 1 /*num outputs*/);
        processor.onaudioprocess = processAudio;

        analyser = context.createAnalyser();
            
        source.connect(context.destination);
        source.connect(analyser);

        analyser.connect(processor);
        processor.connect(context.destination);

        source.noteOn(0);
        disconnectTimeout = setTimeout(disconnect,
                                       source.buffer.duration * 1000 +1000);
    }

    function disconnect() {
        source.noteOff(0);
        source.disconnect(0);
        processor.disconnect(0);
        analyser.disconnect(0);

        disconnectTimeout = 0;
    }

    function processAudio(e) {
        var freqByteData = new Float32Array(analyser.frequencyBinCount);
        analyser.getFloatFrequencyData(freqByteData);
        
        var buf = Module._malloc(freqByteData.length * freqByteData.BYTES_PER_ELEMENT);
        Module.HEAPF32.set(freqByteData, buf >> 2);
        Module._set_frequencies(buf, analyser.minDecibels, analyser.maxDecibels - analyser.minDecibels);
        Module._free(buf);
    }

    function dropEvent(evt) {
        evt.stopPropagation();
        evt.preventDefault();

        if (disconnectTimeout) {
            clearTimeout(disconnectTimeout);
            disconnectTimeout = 0;
        }

        var droppedFiles = evt.dataTransfer.files;
        
        var reader = new FileReader();
        
        reader.onload = function(fileEvent) {
            var data = fileEvent.target.result;
            initAudio(data);
        };

        var infoBox = document.getElementById("info-box");
        infoBox.parentNode.removeChild(infoBox);
        
        reader.readAsArrayBuffer(droppedFiles[0]);
    }

    function handleResult() {
        if (xhr.readyState == 4 /* complete */) {
            switch(xhr.status) {
                case 200: /* Success */
                    initAudio(request.response);
                    break;
                default:
                    break;
            }
            xhr = null;
        }
    }

    function dragOver(evt) {
        evt.stopPropagation();
        evt.preventDefault();
        return false;
    }

    var dropArea = document.getElementById('module');
    dropArea.addEventListener('drop', dropEvent, false);
    dropArea.addEventListener('dragover', dragOver, false);

}


var Module = {
    preRun: [],
    postRun: [],
    onRuntimeInitialized: function() {
        audio();
        Module._run_fmv_with_frequency_provider(document.body.clientWidth, document.body.clientHeight);
    },

    printErr: function(message) {
        console.error(Array.prototype.slice.call(arguments).join(' '));
    },

    print: function(message) {
        console.log(Array.prototype.slice.call(arguments).join(' '));
    },

    canvas: document.getElementById('module'),

    setStatus: function(message) {
        var status = document.getElementById('status');
        if(status) status.innerHTML = message;
    },

    setStatusDescription: function(message) {
        var statusDescription = document.getElementById('statusDescription');
        if(statusDescription) statusDescription.innerHTML = message;
    },

    totalDependencies: 0,

    monitorRunDependencies: function(left) {
        this.totalDependencies = Math.max(this.totalDependencies, left);

        if(left) {
            Module.setStatus('Downloading...');
            Module.setStatusDescription((this.totalDependencies - left) + '/' + this.totalDependencies);
        } else {
            Module.setStatus('Download complete');
            Module.setStatusDescription('');
        }
    }
};

Module.setStatus('Downloading...');


