videoqueue = [];

window.onload = async function(e){
    video = document.getElementById('video');
    video.latencyHint = 0;
    if (window.MediaSource) {
	var mediaSourceVideo = new MediaSource();
        mediaSourceVideo.addEventListener('sourceopen', sourceOpenVideo);
        video.src = URL.createObjectURL(mediaSourceVideo);
	video.load();
    } else {
        console.log("The Media Source Extensions API is not supported.")
    }
    audio = document.getElementById('audio');
    audio.latencyHint = 0;
    if (window.MediaSource) {
        var mediaSource = new MediaSource();
        mediaSource.addEventListener('sourceopen', sourceOpen);
        audio.src = URL.createObjectURL(mediaSource);
        audio.load();
//	URL.revokeObjectURL(audio.src);
    } else {
        console.log("The Media Source Extensions API is not supported.")
    }
}

function sourceOpenVideo(e) {
    const audio = document.getElementById('video');
    var mime = 'video/mp4; codecs="avc1.64001E"';
    var mediaSourceVideo = this;
    var firstplay = true;
    var videoSourceBuffer = mediaSourceVideo.addSourceBuffer(mime);

    if ( !mediaSourceVideo ) {
	console.log( "no MediaSource" );
	return;
    }

    videoqueue.push = function( buffer ) {
        Array.prototype.push.call( this, buffer )
	videobump();
    }

    var playing = false;

    var resets = 0;
    // document.getElementById('videobuffer').innerHTML = "resets: " + resets;

    videobump = function() {
        if (videoqueue.length > 0 && !videoSourceBuffer.updating) {
	    videoSourceBuffer.appendBuffer(videoqueue.shift());

	    if ( videoSourceBuffer.buffered.length > 0 ) {
		var buffer_duration = (videoSourceBuffer.buffered.end(0) - video.currentTime);
		// document.getElementById('videobuffer').innerHTML = "buffer: " + (24*buffer_duration).toFixed(0) + " frames";

		vid_ws.send("buffer " + buffer_duration.toFixed(3));

		if ( playing && (buffer_duration > 0.5) ) {
		    video.currentTime = videoSourceBuffer.buffered.end(0) - 0.25;
		    resets++;
		    // document.getElementById('videoresets').innerHTML = "resets: " + resets;
		}
	    }
	}
    }

    set_live = function(name) {
	vid_ws.send("live " + name );
    }

    add_control = function(name) {
	// document.getElementById('controls').innerHTML += "<button onclick='set_live(this.id)' type='button' id='" + name + "'>" + name + "</button>";
    }

    vid_ws = new WebSocket("wss://stagecast.org:8400");
    vid_ws.binaryType = 'arraybuffer';
    vid_ws.onmessage = function( e ) {
	var type_byte = new DataView(e.data, 0, 1).getUint8(0);
	var rest = e.data.slice(1);
	if ( type_byte == 1 ) {
	    // document.getElementById('message').innerHTML = new TextDecoder("utf-8").decode(rest);
	    return;
	}
	if ( type_byte == 2 ) {
	    add_control( new TextDecoder("utf-8").decode(rest) );
	    return;
	}
	if ( type_byte == 3 ) {
	    return;
	}

	videoqueue.push(rest);

	if ( firstplay && videoSourceBuffer.buffered.length > 0 ) {
            video.currentTime = videoSourceBuffer.buffered.start(0);
	    firstplay = false;
	}
    }

    videoSourceBuffer.addEventListener('abort', function(e) {
	// document.getElementById('status').innerHTML = "playback abort";
        console.log('video source buffer abort:', e);
    });
    videoSourceBuffer.addEventListener('error', function(e) {
	// document.getElementById('status').innerHTML = "playback error";
	vid_ws.onmessage = function() {}
    });
    videoSourceBuffer.addEventListener('updateend', videobump);

    video.oncanplay = function() {
	var play_promise = video.play();

	if (play_promise !== undefined) {
	    play_promise.then(function() {
		// playback started; only render UI here
		// document.getElementById('status').innerHTML = 'Playing...';
		playing = true;
	    }).catch(function(error) {
		// playback failed
		// document.getElementById('status').innerHTML = 'Play failed. Please click play to begin playing.';
		playing = false;
	    });
	}
    };
}



/***** ******/


queue = [];

function sourceOpen(e) {
    const audio = document.getElementById('audio');
    var mime = 'audio/webm; codecs="opus"';
    var mediaSource = this;
    var firstplay = true;
    var sourceBuffer = mediaSource.addSourceBuffer(mime);

    if ( !mediaSource ) {
	console.log( "no MediaSource" );
	return;
    }

    queue.push = function( buffer ) {
        Array.prototype.push.call( this, buffer )
	bump();
    }

    var playing = false;

    var resets = 0.0;

    setInterval(function(){
	if ( sourceBuffer.buffered.length > 0 ) {
	    var buffer_duration = sourceBuffer.buffered.end(0) - audio.currentTime;
	    snd_ws.send("buffer " + buffer_duration.toFixed(3));
	    // document.getElementById('buffer').innerHTML = "Buffer: " + (24*buffer_duration).toFixed(0) + " video frames";
	}
    }, 50);

    bump = function() {
        if (queue.length > 0 && !sourceBuffer.updating) {
            sourceBuffer.appendBuffer(queue.shift());

	    if ( sourceBuffer.buffered.length > 0 ) {
		var buffer_duration = sourceBuffer.buffered.end(0) - audio.currentTime;

		if ( buffer_duration > 0.6 ) {
		    audio.currentTime = sourceBuffer.buffered.end(0) - 0.3;
		    resets++;
		    // document.getElementById('resets').innerHTML = "resets: " + resets;
		}
	    }
	}
    }

    snd_ws = new WebSocket("wss://stagecast.org:8081");
    snd_ws.binaryType = 'arraybuffer';
    snd_ws.onmessage = function( e ) {
	var type_byte = new DataView(e.data, 0, 1).getUint8(0);
	var rest = e.data.slice(1);

	if ( type_byte == 1 ) {
	    document.getElementById('playing').innerHTML = new TextDecoder("utf-8").decode(rest);
	    return;
	}

	queue.push(rest);

//	console.log("buffered: " + sourceBuffer.buffered.start(0) + " to " + sourceBuffer.buffered.end(0) );

	if ( firstplay && sourceBuffer.buffered.length > 0 ) {
            audio.currentTime = sourceBuffer.buffered.start(0);
	    firstplay = false;
	}
    }

    sourceBuffer.addEventListener('abort', function(e) {
        console.log('audio source buffer abort:', e);
    });
    sourceBuffer.addEventListener('error', function(e) {
        console.log('audio source buffer error:', e);
    });
    sourceBuffer.addEventListener('updateend', bump);

    audio.oncanplay = function() {
	var play_promise = audio.play();

	if ( play_promise == undefined ) {
	    console.log( "undefined promise" );
	} else {
	    play_promise.then(function() {
		playing = true;
		audio.oncanplay = function() {};
	    }).catch(function(error) {
		console.log( "play error: " + error );
	    });
	}
    }
}

function select(feed) {
    snd_ws.send("live " + feed );
}
