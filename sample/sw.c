#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "protocol.h"

#define MAX_SEQ 7					// 2*n-1
#define NR_BUFS ((MAX_SEQ + 1)/ 2)

typedef enum {frame_arrival, cksum_err, timeout, network_layer_ready, ack_timeout} event_type;
bool no_nak = true;					// tidak ada NAK yang sudah dikirim
seq_nr oldest_frame = MAX_SEQ + 1;	// hanya untuk simulasi

static bool between(seq_nr a, seq_nr b, seq_nr c){
	// return if a <= b < c
	return ((a <= b) && (b < c)) || ((a <= b) && (c < a)) || ((b < c) && (c < a));
}

static void send_frame(frame_kind fk, seq_nr frame_nr, seq_nr frame_expected, packet buffer[]){
	// Membuat dan mengirim data, acknowledge, atau nak frame

	frame s;					// variabel

	s.kind = fk 				// kind == data, ack, atau nak
	if (fk == data) s.info = bufer[frame_nr % NR_BUFS];
	s.seq = frame_nr;			// hanya digunakan untuk frame data
	s.ack = (frame_expected MAX_SEQ) % (MAX_SEQ + 1);
	if (fk == nak) no_nak = false;	// setiap frame 1 nak
	to_physical_layer(&s);			// transmit frame
	if (fk == data) start_timer(frame_nr % NR_BUFS);
	stop_ack_timer();				// tidak membutuhkan ack frame yang terpisah
}

void sliding_window(){
	seq_nr ack_expected;			// lower edge of sender's window
	seq_nr next_frame_to_send;		// upper edge of sender's window + 1
	seq nr frame expected; 			// lower edge of receiver’s window
	seq nr too far; 				// upper edge of receiver’s window + 1
	int i; 							// index into buffer pool
	frame r; 						// scratch variable
	packet out_buf[NR_BUFS]; 		// buffers for the outbound stream
	packet in_buf[NR_BUFS]; 		// buffers for the inbound stream
	bool arrived[NR_BUFS];	 		// inbound bit map
	seq_nr nbuffered; 				// how many output buffers currently used
	event_type event;

	enable_network_layer();			// inisialisasi
	ack_expected = 0;				// ack selanjutna di dalam inbound stream
	next_frame_to_send = 0;			// jumlah frame yang akan dikirim selanjutnya
	frame_expected = 0;
	too_far = NR_BUFS;
	nbuffered = 0;					// inisialisasi buffer kosong
	memset(arrived, false, sizeof(arrived));

	while(true){
		wait_for_event(&event)		// cek event
		switch (event) {
			case network_layer_ready :	// terima, simpan dan transmit frame baru
				nbuffered += 1; 		// mengembangkan window
				next_frame_to_send(&out_buf[next_frame_to_send % NR_BUFS]);		// detch new packet
				send_frame(data, next_frame_to_send, frame_expected, out_buf);	// transmit frame
				next_frame_to_send++;	// advaance upper window edge
				break;
			case frame_arrival : 		// data atau control frame diterima
				from_physical_layer(&r);
				if(r.kind == data){
					// frame yang tdak rusak akan diterima
					if ((r.seq != frame_expected) && no_nak){
						send_frame(nak, 0, frame_expected, out_buf)
					} else stop_ack_timer();

					if (between(frame_expected, r.seq, too_far) && (arrived[r.seq % NR_BUFS] == false)){
						// frame mungkin akan diterima dalam berbagai urutan

						arrived[r.seq % NR_BUFS] = true; // mark buffer penuh
						in_buf[r.seq % NR_BUFS] = r.info // memasukan data ke dalam buffer

						while (arrived[frame_expected % NR_BUFS]){
							// mengirimkan frame dan mengupdate window

							to_network_layer(&in_buf[frame_expected % NR_BUFS]);
							no_nak = true;
							arrived[frame_expected % NR_BUFS] = false;
							frame_expected++;	// advance lower edge of receiver window
							too_far++;			// advance upper edge of receiver window
							stop_ack_timer();	// untuk melihat apakah ack terpisah dibutuhkan
						}
					}
				}
				if((r.kind == nak) && between(ack_expected, (r.ack+1) % (MAX_SEQ+1), next_frame_to_send)){
					send_frame(data, (r.ack+1) % (MAX_SEQ+1), frame_expected, out_buf);
				}
				while(between(ack_expected, r.ack, next_frame_to_send)){
					nbuffered -= 1;			// handle piggybacked ack
					stop_timer(ack_expected % NR_BUFS); // frame arrived intact
					ack_expected++			// advance lower edge if sender's wiindow
				}
				break;
			case cksum_err :
				if(no_nak) send_frame(nak, 0, frame_expected, out_buf); // frame rusak
				break;
			case timeout :
				send_frame(data, oldest_frame, frame_expected, out_buf); // timeout terjadi
				break;
			case ack_timeout :
				send_frame(ack, 0, frame_expected, out_buf)	// ack timeout, mengirimm ack kembali
		}

		if(nbuffered < NR_BUFS) enable_network_layer();
		else disable_network_layer();
	}

}
